#include "mbed.h"
#include "coapServer.h"
#include "radioWrapper.h"
#include <string>
#include "dbg.h"
#include "msg.h"
#include "itoa.h"

int channel = 4;

unsigned long long txAdress = 0xABCDEF08;
unsigned long long rxAdress[] = {0xABCDEF11, 0xABCDEF12, 0xABCDEF13};

Serial pc(SERIAL_TX, SERIAL_RX);
Serial wifi(PA_11, PA_12);
DigitalOut send(PC_8);

#define PCDBG(...) if(1){ pc.printf(__VA_ARGS__); pc.printf("\r\n");}
#define BINDBG(buff, len) if(1){ print_bin(buff, len); }
#define HEXDBG(buff, len) if(1){ print_hex(buff, len);}

//Number of retransmissions
#define RETRANSMISSION 10
//Timeout in ms
#define TIMEOUT 2000

//Length of PUT (data) response
#define PUT_ANS_LEN 5
//Length of GET (conf) response
#define GET_ANS_LEN 10

int read_finished = 0;
uint16_t messageID = 1;

void print_bin(uint8_t* buff, int len) {
    char str[len];
    for(int i = 0; i < len; ++i) {
        if((buff[i] >= 'a' && buff[i] <= 'z') || (buff[i] >= 'A' && buff[i] <= 'Z') || (buff[i] >= '0'&& buff[i] <= '9')) {
            str[i] = buff[i];
        } else {
            str[i] = '.';
        }
    }
    PCDBG("%s", str);
}

void print_hex(uint8_t* buff, int len) {
    for(int i = 0; i < len; ++i) {
        pc.printf("%X ", buff[i]);
    }
    pc.printf("\r\n");
}

void read_cb(int arg) {
    (void)arg;
    read_finished = 1;
}

CoapPDU* read(uint8_t* rx_buff, int len) {
    int ret;
    PCDBG("Reading");
    read_finished = 0;
    Timer t;
    t.start();
    while(!wifi.readable() && t.read_ms() < TIMEOUT) {}
    if(wifi.readable()) 
        //ret = wifi.read(rx_buff, len, event_callback_t(&read_cb));
        wifi.gets((char*)rx_buff, len + 1);
    else {
        t.stop();
        return NULL;
    }
    PCDBG("Started reading");
    //t.reset();
    //t.start();
    //while(read_finished == 0 && t.read_ms() < TIMEOUT) {}
    //while(read_finished == 0) {}
    t.stop();
    PCDBG("Finished Reading, ret = %d, read_finished = %d, time = %d", ret, read_finished, t.read_ms())
    HEXDBG(rx_buff, len);
    //if(read_finished != 1) {
//        wifi.abort_read();
//        read_finished = 0;
//        return NULL;
//    }
    read_finished = 0;
    CoapPDU* recvPDU = new CoapPDU(rx_buff, len);
    PCDBG("PDU created");
    if(recvPDU->validate()) {
        PCDBG("Validated");
        recvPDU->printHuman();
        PCDBG("Code = %d", recvPDU->getCode());
        PCDBG("Payload:");
        HEXDBG(recvPDU->getPayloadPointer(), len - 6);
        return recvPDU;
    }
    return NULL;
}

void send_cb(int arg) {
    (void)arg;
    send.write(1);
    wait(0.05);
    send.write(0);
}

int sendd(uint8_t* buff, int len) {
    while(!wifi.writable()) {
        //PCDBG("Wifi not wirtable");
        //wait(0.5);
    }
    PCDBG("Sending:");
    HEXDBG(buff, len);
    //For some mysterious reason it doesn't print first character
    uint8_t buf2[len+1];
    std::memcpy(buf2+1, buff, len);
    buf2[0] = 0;
    int ret = wifi.write(buf2, len + 1, event_callback_t(&send_cb));
    PCDBG("Sending initiated");
    return ret;
}

CoapPDU* send_or_fail(uint8_t* buff, int len, uint8_t* rx_buff, int ans_len) {
    int ret;
    CoapPDU* ans;
    for(int i = 0; i < RETRANSMISSION; ++i) {
        ret = sendd(buff, len);
        if(ret < 0) {
            PCDBG("Send Failed");
            return NULL;
        }
        ans = read(rx_buff, ans_len);
        if(ans == NULL) {
            PCDBG("Read Failed");
        } else if(ans->getType() != CoapPDU::COAP_ACKNOWLEDGEMENT || ans->getMessageID() != messageID) {
            PCDBG("Wrong answer IS: type = %d, id = %d SHOULD BE: type = %d, id = %d", ans->getType(), ans->getMessageID(), CoapPDU::COAP_ACKNOWLEDGEMENT, messageID);
            delete ans;
        } else {
            PCDBG("ACK get");
            return ans;
        }
    }
    PCDBG("Retransmission limit reached");
    return NULL;
}


void preparePDU(CoapPDU& coapPDU) {
    coapPDU.setVersion(1);
    coapPDU.setType(CoapPDU::COAP_CONFIRMABLE);
    coapPDU.setMessageID(++messageID);
    coapPDU.setToken((uint8_t*)"A", 1);
}

int send_data(int type, int data) {
    uint8_t buff[5];
    uint8_t rx_buff[PUT_ANS_LEN];
    Msg::construct_data_msg(type, data, buff, 5);
    CoapPDU pdu = CoapPDU();
    preparePDU(pdu);
    pdu.setCode(CoapPDU::COAP_PUT);
    pdu.setURI((char*)"data",4);
    pdu.setPayload(buff, 5);
    CoapPDU* ans =  send_or_fail(pdu.getPDUPointer(), pdu.getPDULength(), rx_buff, PUT_ANS_LEN);
    if(ans == NULL) return -1;
    delete ans;
    return 0;
}

int get_config(uint8_t type) {
    uint8_t buff[1];
    uint8_t rx_buff[GET_ANS_LEN];
    PCDBG("Gonna get the conf for type %d", type);
    buff[0] = type;
    CoapPDU pdu = CoapPDU();
    preparePDU(pdu);
    pdu.setCode(CoapPDU::COAP_POST);
    pdu.setURI((char*)"conf",4);
    pdu.setPayload(buff, 1);
    CoapPDU* ans = send_or_fail(pdu.getPDUPointer(), pdu.getPDULength(), rx_buff, GET_ANS_LEN);
    if(ans == NULL) {
        PCDBG("Failed to get config");
        return -1;
    }
    uint8_t* config = ans->getPayloadPointer();
    int conf = 0;
    int a = 1;
    for(int i = 3; i >= 0; --i) {
        conf += config[i] * a;
        a *= 256;
    }
    delete ans;
    PCDBG("New config for type %d is %d", type, conf);
    return conf;
}

InterruptIn button(USER_BUTTON);
bool start = false;

void send_packet() {
    start = true;
}

int main() {
    pc.baud(115200);
    wifi.baud(115200);
    std::string uris[] = {"/dist", "/temp", "/hum", "/moist", "/conf/dist", "/conf/temp", "/conf/hum", "/conf/moist"};
    CoapServer coapServer = CoapServer(32, 1000, uris, 8, channel, txAdress, rxAdress);
    button.rise(&send_packet);
    uint8_t buffer[32];
    CoapServer::METHOD method;
    int uriNum = -1;
    pc.printf("Server initialized \r\n");
    int pipe = -1;
    while (1) {
        while (start) {
            int ret = coapServer.listen(buffer, 32, &method, &uriNum, &pipe);
            pc.printf("Code    : %d \r\n", ret);
            if (ret >= 0) {
                pc.printf("Recieved: %s \r\n", buffer);
                pc.printf("*****Pipe: %d", pipe);
                int resLen = 0;
                if (method == CoapServer::POST) {
                    resLen = coapServer.respond(uriNum, NULL, 0, CoapPDU::COAP_CREATED, pipe);
                    int data = 0;
                    uint8_t sensor = 1;
                    Msg::deconstruct_data_msg(&sensor, &data, buffer, ret);
                    send_data(uriNum % 4, data);
                    switch (uriNum % 4) {
                        case 0:
                        DBG("dist %d", data);
                            break;
                        case 1:
                        DBG("TEMP %d", data);
                            break;
                        case 2:
                        DBG("HUM %d", data);
                            break;
                        case 3:
                        DBG("MOIST %d", data);
                            break;    
                    }
                } else if (method == CoapServer::GET) {
                    uint8_t response[10];
                    int conf = get_config(uriNum % 4);
                    Msg::construct_data_msg(0, conf, response, 10);
                    pc.printf("*****response:\r\n");
                    for (int i = 0; i < 10; i++) {
                        pc.printf("%d ", response[i]);
                    }
                    pc.printf("\r\n");
                    resLen = coapServer.respond(uriNum, response, 10, CoapPDU::COAP_CONTENT, pipe);
                }
                DBG("RESPONDED WITH %d", resLen);
            }
        }
    }
}