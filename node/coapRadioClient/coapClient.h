//
// Created by Kamil Mykitiuk on 2019-01-17.
//

#ifndef COAPCLIENT_H
#define COAPCLIENT_H


#include <cstdint>
#include "cantcoap.h"
#include "radioWrapper.h"

class CoapClient {
public:
    enum ERROR {
        SERVER_RESPONSE_AMBIGOUS = -1,
        SERVER_TIMED_OUT = -2,
        SMALL_BUFFER = -3,
        RADIO_NOT_WORKING = -4
    };

private:
    uint8_t token[1];
    uint16_t message_counter;
    int listeningTimeout;
    int retransmissionLimit;
    RadioWrapper radioWrapper;


public:
    CoapClient(uint16_t counter, uint8_t* token, int retransmissionLimit, int timeout, int channel, unsigned long long rx_address,
               unsigned long long tx_address);

    int get(uint8_t buffer[], int len, char* uri);

    int post(uint8_t buffer[], int len, char* uri);

private:
    uint16_t preparePDU(CoapPDU& coapPDU);

    bool isAck(CoapPDU& coapPDU, uint16_t message_id, char* uri);

    bool hasGetResponse(CoapPDU& response);

    bool hasPostResponse(CoapPDU& response);
};

#endif //COAPCLIENT_H
