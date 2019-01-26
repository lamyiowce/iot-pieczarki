#include "radioWrapper.h"
#include "mbed.h"
#include "dbg.h"

int RadioWrapper::read(uint8_t* buffer, int len, int timeout) {
    Timer t;
    t.start();
    // check if buffor is large enough to conaint packet.
    if (len < packetSize()) {
        return -1;
    }
    while(t.read_ms() < timeout) {
        if(radio.readable(NRF24L01P_PIPE_P1)) {
            DBG("%d", t.read_ms());
            uint8_t message[packetSize()];
            int ret = radio.read(NRF24L01P_PIPE_P1, (char*) message, packetSize());
            DBG("RECIEVED %d, %s \r\n", ret, buffer);
            t.stop();
            if(ret>0) {
                uint8_t encodedLen = message[0];
                std::memcpy(buffer, message+1, encodedLen);
                return encodedLen;
            } else {
                return ret;
            }
        }
    }
    t.stop();
    return 0;
}

int RadioWrapper::write(uint8_t* buffer, uint8_t len) {
    // check if buffor is small enough to send in one package + len
    if (len > packetSize()) {
        return -1;
    }

    uint8_t message[packetSize()];
    message[0] = len;
    std::memcpy(message+1, buffer, len);
    int ret = radio.write(NRF24L01P_PIPE_P0, (char*) message, packetSize());
    return ret;
}

RadioWrapper::RadioWrapper(int channel, unsigned long long rx_address, unsigned long long tx_address) :
        radio(PB_15, PB_14, PB_13, PB_12, PB_1, PB_2) {
    radio.powerDown();
    radio.powerUp();

    radio.setAirDataRate(DATA_RATE);
    radio.setRfOutputPower(POWER);
    radio.setRfFrequency(NRF24L01P_MIN_RF_FREQUENCY + 4 * channel);

    radio.setCrcWidth(NRF24L01P_CRC_8_BIT);

    radio.setTxAddress(tx_address, 4);
    radio.setRxAddress(tx_address, 4, NRF24L01P_PIPE_P0);
    radio.setRxAddress(rx_address, 4, NRF24L01P_PIPE_P1);
    radio.setTransferSize(packetSize(), NRF24L01P_PIPE_P0);
    radio.setTransferSize(packetSize(), NRF24L01P_PIPE_P1);

    radio.setReceiveMode();
    radio.enable();

    // Display the (default) setup of the nRF24L01+ chip
    DBG( "nRF24L01+ Frequency    : %d MHz\r\n",  radio.getRfFrequency() );
    DBG( "nRF24L01+ Output power : %d dBm\r\n",  radio.getRfOutputPower() );
    DBG( "nRF24L01+ Data Rate    : %d kbps\r\n", radio.getAirDataRate() );
    DBG( "nRF24L01+ TX Address   : 0x%010llX\r\n", radio.getTxAddress() );
    DBG( "nRF24L01+ RX0 Address   : 0x%010llX\r\n", radio.getRxAddress(NRF24L01P_PIPE_P0) );
    DBG( "nRF24L01+ RX1 Address   : 0x%010llX\r\n", radio.getRxAddress(NRF24L01P_PIPE_P1) );
}
