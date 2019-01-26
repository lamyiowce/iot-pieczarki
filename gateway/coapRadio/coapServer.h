//
// Created by Kamil Mykitiuk on 2019-01-18.
//

#ifndef SLAVE_COAPSERVER_H
#define SLAVE_COAPSERVER_H

#include "cantcoap.h"
#include "radioWrapper.h"
#include <string>
#include <vector>

class CoapServer {
public:

    enum ERROR {
        SMALL_BUFFER = -3,
        RADIO_NOT_WORKING = -4,
        UNKNOWN_URI = -5,
        COAP_ERROR = -6,
        SILENT = -7
    };

    enum METHOD {
        GET = 1,
        POST = 2
    };

private:
    unsigned queueMax;
    int lastMessageIndex;
    std::vector<uint16_t> idQ;
    std::vector<uint8_t>  tokenQ;
    int listeningTimeout;
    RadioWrapper radioWrapper;
    int urisNumber;
    std::string* uris;


public:
    CoapServer(unsigned queueMax, int timeout, std::string* uris, int urisNumber, int channel,
                       unsigned long long tx_address, unsigned long long rx_addresses[]);


    int listen(uint8_t* buffer, int len, METHOD* requestMethod, int* uriNumber, int* pipe);

    int respond(int uri, uint8_t* buffer, int len, CoapPDU::Code responseCode, int pipe);

private:
    int ackRepeatedPost(CoapPDU& request, int pipe);
    int nextIndex();

};

#endif //SLAVE_COAPSERVER_H
