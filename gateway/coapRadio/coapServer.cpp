//
// Created by Kamil Mykitiuk on 2019-01-18.
//

#include "coapServer.h"
#include "dbg.h"

int CoapServer::nextIndex() {
    this->lastMessageIndex = (this->lastMessageIndex+1) % this->queueMax;
    return this->lastMessageIndex;
}

int CoapServer::ackRepeatedPost(CoapPDU& request, int pipe) {
    CoapPDU response = CoapPDU();
    response.setVersion(1);
    response.setType(CoapPDU::COAP_ACKNOWLEDGEMENT);
    response.setCode(CoapPDU::COAP_CREATED);
    char requestUri[32];
    int requestUriLen = 0;
    request.getURI(requestUri, 32, &requestUriLen);
    if(response.setURI(requestUri, requestUriLen) != 0) {
        return COAP_ERROR;
    }
    if(radioWrapper.write(response.getPDUPointer(), response.getPDULength(), pipe) < 0) {
        return RADIO_NOT_WORKING;
    }
    return SILENT;
}

int CoapServer::listen(uint8_t* buffer, int len, METHOD* requestMethod, int* uriNumber, int* pipe) {
    uint8_t requestBuffer[radioWrapper.packetSize()];
    int ret = radioWrapper.read(requestBuffer, radioWrapper.packetSize(), this->listeningTimeout, pipe);
    DBG("READ: %d", ret);
    if (ret < 0) {
        return RADIO_NOT_WORKING;
    } else if(ret > 0) {
        CoapPDU request(requestBuffer, radioWrapper.packetSize(), ret);
        if (request.validate()) {
            //request.printHuman();
            for (int i = 0; i < idQ.size(); ++i) { // check if messageId is known
                if(idQ[i] == request.getMessageID()) {
                    if(request.getCode() == CoapPDU::COAP_POST) { // dla posta odpowiadamy pustym
                        return ackRepeatedPost(request, *pipe);
                    }
                } else { // geta obsługujemy od nowa
                    break;
                }
            }

            // zapamiętaj dane
            int index = nextIndex();
            this->idQ[index] = request.getMessageID();
            this->tokenQ[index] = request.getTokenPointer()[0];

            if (request.getPayloadLength() <= len) {
                std::memcpy(buffer, request.getPayloadPointer(), static_cast<size_t>(request.getPayloadLength()));

                if (request.getCode() == CoapPDU::COAP_GET) {
                    *requestMethod = GET;
                } else if (request.getCode() == CoapPDU::COAP_POST) {
                    *requestMethod = POST;
                }

                char requestUri[32];
                int requestUriLen = 0;
                request.getURI(requestUri, 32, &requestUriLen);
                bool foundMatchingUri = false;
                DBG("URI: %s %d", requestUri, requestUriLen);

                for (int i = 0; i < urisNumber; ++i) {
                    if(std::strcmp(requestUri, this->uris[i].data()) == 0) {
                        *uriNumber = i;
                        foundMatchingUri = true;
                    }
                }
                if(!foundMatchingUri) {
                    return UNKNOWN_URI;
                }

                return request.getPayloadLength();
            } else {
                return SMALL_BUFFER;
            }
        }
    }
    return SILENT;
}

int CoapServer::respond(int uri, uint8_t* buffer, int len, CoapPDU::Code responseCode, int pipe) {
    CoapPDU response = CoapPDU();
    response.setVersion(1);
    response.setType(CoapPDU::COAP_ACKNOWLEDGEMENT);
    response.setMessageID(this->idQ[this->lastMessageIndex]);
    uint8_t token = this->tokenQ[this->lastMessageIndex];
    response.setToken(&token, 1);
    response.setCode(responseCode);
    response.setURI((char*) uris[uri].data(), (int)uris[uri].size());
    if(len > 0) {
        response.setPayload(buffer, len);
    }
    int ret = radioWrapper.write(response.getPDUPointer(), response.getPDULength(), pipe);
    if(ret < 0) {
        return RADIO_NOT_WORKING;
    }
    return ret;
}

CoapServer::CoapServer(unsigned queueMax, int timeout, std::string* uris, int urisNumber, int channel,
                       unsigned long long tx_address, unsigned long long rx_addresses[]) : queueMax(queueMax), listeningTimeout(timeout),radioWrapper(channel, tx_address, rx_addresses), urisNumber(urisNumber), uris(uris){
    idQ.resize(queueMax, 0);
    tokenQ.resize(queueMax, 0);
}