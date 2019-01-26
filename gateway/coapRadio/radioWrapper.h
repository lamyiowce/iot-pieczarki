#pragma once

#include "nRF24L01P.h"

//***************************************************************************//
// !!! SETTINGS !!!
#define DATA_RATE       NRF24L01P_DATARATE_1_MBPS

#define POWER           NRF24L01P_TX_PWR_ZERO_DB

//***************************************************************************//

class RadioWrapper {
private:
    nRF24L01P radio;    // mosi, miso, sck, csn, ce, irq

public:
    RadioWrapper(int channel, unsigned long long tx_address,  unsigned long long rx_addresses[]);

    int packetSize() {
        return 32;
    }

    /*a
    read bytes and saves them to given buffer turncating it.
    @len
    @return number of read bytes, -1 if error
    */
    int read(uint8_t* buffer, int len, int timeout, int* pipe);
    /*
    send bytes from given buffor
    @return number of writeen bytes, -1 if error.
    */
    int write(uint8_t* buffer, uint8_t len, int pipe);

    private:
    int readFromPipe(uint8_t* buffer, int len, int pipe);
};