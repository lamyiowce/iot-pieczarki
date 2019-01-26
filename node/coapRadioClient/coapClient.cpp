//
// Created by Kamil Mykitiuk on 2019-01-17.
//

#include "coapClient.h"
#include "dbg.h"

bool match_uri(CoapPDU& coapPDU, char* uri) {
    char respone_uri[32];
    int len = 0;
    coapPDU.getURI(respone_uri, 32, &len);
    return strcmp(uri, respone_uri) == 0;
}


bool isExpectedResponse(CoapPDU& response, uint16_t message_id, char* uri) {
    return match_uri(response, uri) && response.getType() == CoapPDU::COAP_ACKNOWLEDGEMENT &&
           response.getMessageID() == message_id;
}

bool CoapClient::isAck(CoapPDU& response, uint16_t message_id, char* uri) {
    if (response.validate()) {
        response.printHuman();
        DBG("WANNA: %d %s", message_id, uri);
        if (isExpectedResponse(response, message_id, uri)) {
            return true;
        }
    }
    return false;
}

bool CoapClient::hasGetResponse(CoapPDU& response) {
    return std::memcmp(response.getTokenPointer(), this->token, static_cast<size_t>(response.getTokenLength())) == 0 &&
           response.getCode() == CoapPDU::COAP_CONTENT;
}

bool CoapClient::hasPostResponse(CoapPDU& response) {
    return response.getCode() == CoapPDU::COAP_CREATED &&
           std::memcmp(response.getTokenPointer(), this->token, static_cast<size_t>(response.getTokenLength())) == 0;
}


/* send get request to given uri of connected server
 *
 * @return number of saved bytes in provided buffer or
 * -1 if server responded ambiguously;
 * -2 if server timed out;
 * -3 if provided buffer is to small to save response,
 * -4 if if radio is not functioning properly
 * */
int CoapClient::get(uint8_t* buffer, int len, char* uri) {
    Timer t;
    CoapPDU coapPDU = CoapPDU();
    uint16_t messageId = preparePDU(coapPDU);
    this->message_counter +=4;
    coapPDU.setCode(CoapPDU::COAP_GET);
    coapPDU.setURI(uri);

    uint8_t returnBuffer[radioWrapper.packetSize()];
    int TIMEOUT = this->listeningTimeout;
    int timeout = TIMEOUT;
    for (int i = 0; i < this->retransmissionLimit; i++) {
        while (timeout > 0) {
            int ret = radioWrapper.write(coapPDU.getPDUPointer(), coapPDU.getPDULength());  // send empty payload to get uri
            if (ret < 0) {
                return RADIO_NOT_WORKING;
            }
            t.start();
            int readLen = radioWrapper.read(returnBuffer, radioWrapper.packetSize(), timeout); // add timeout param
            t.stop();
            timeout -= t.read_ms();
            t.reset();
            if (readLen > 0) { // if something is recieved process it
                CoapPDU response(returnBuffer, len, readLen);
                if (isAck(response, messageId, uri) && hasGetResponse(response)) {
                    DBG("RESPONSE: %d", readLen);
                    response.printHuman();
                    if (response.getPayloadLength() <= len) {
                        std::memcpy(buffer, response.getPayloadPointer(),
                                    static_cast<size_t>(response.getPayloadLength()));
                        return response.getPayloadLength();
                    } else {
                        return SMALL_BUFFER;
                    }
                } else {
                    DBG("NIEMOJE");
                    DBG("PAYLOAD: %s", response.getPayloadPointer());
                    continue;
                }
            } else if (readLen < 0) {
                return RADIO_NOT_WORKING;
            }
        }
        TIMEOUT *= 2;
        timeout = TIMEOUT;
    }

    return
            SERVER_TIMED_OUT;
}

/* send post request to given uri of connected server
 *
 * @return 0 if operation has been completed successfully
 * -1 if server responded ambiguously;
 * -2 if server timed out;
 * -4 if radio is not working properly
 * */
int CoapClient::post(uint8_t* buffer, int len, char* uri) {
    Timer t;
    CoapPDU coapPDU = CoapPDU();
    uint16_t messageId = preparePDU(coapPDU);
    this->message_counter +=4;
    coapPDU.setCode(CoapPDU::COAP_POST);

    coapPDU.setURI(uri);
    coapPDU.setPayload(buffer, len);
    uint8_t returnBuffer[radioWrapper.packetSize()];
    int TIMEOUT = this->listeningTimeout;
    int timeout = TIMEOUT;
    for (int i = 0; i < this->retransmissionLimit; i++) {
        while(timeout > 0) {
            int ret = radioWrapper.write(coapPDU.getPDUPointer(), coapPDU.getPDULength());
            if (ret < 0) {
                return RADIO_NOT_WORKING;
            }
            t.start();
            int readLen = radioWrapper.read(returnBuffer, radioWrapper.packetSize(), timeout); // add timeout param
            t.stop();
            timeout -= t.read_ms();
            t.reset();
            if (readLen > 0) { // if something is recieved process it
                CoapPDU response(returnBuffer, len, readLen);
                if (isAck(response, messageId, uri) && hasPostResponse(response)) {
                    return 0;
                } else { // if server hasn't responded properly
                    DBG("NIEMOJE");
                    DBG("PAYLOAD: %s", response.getPayloadPointer());
                    continue;
                }
            } else if (readLen < 0) {
                return RADIO_NOT_WORKING;
            }
        }
        TIMEOUT *= 2;
        timeout = TIMEOUT;
    }
    return SERVER_TIMED_OUT;
}

uint16_t CoapClient::preparePDU(CoapPDU& coapPDU) {
    coapPDU.setVersion(1);
    coapPDU.setType(CoapPDU::COAP_CONFIRMABLE);
    coapPDU.setMessageID(this->message_counter);
    coapPDU.setToken(this->token, 1);
    return this->message_counter;
}

CoapClient::CoapClient(uint16_t counter, uint8_t* token, int retransmissionLimit, int timeout, int channel, unsigned long long rx_address,
                       unsigned long long tx_address):
        listeningTimeout(timeout), retransmissionLimit(retransmissionLimit),
        radioWrapper(channel, rx_address, tx_address), message_counter(counter)  {
    std::memcpy(this->token, token, 1);
}