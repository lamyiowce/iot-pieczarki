//#include "sensor.h"
//#include "mbed.h"
//#include "msg.h"
//#include "dbg.h"
//#include "coapClient.h"
//#include <string>
//
//// https://os.mbed.com/users/Sissors/code/WakeUp/
//#include "WakeUp.h"
//
//#include "DHT11.h"
//#include "humidityMeter.h"
//#include "tempMeter.h"
//
//#define DATA_BUFFER_LEN 10
//#define BUFFER_LEN 32
//#define CHANNEL 4
//#define RX_ADDRESS 0xABCDEF08
//#define TX_ADDRESS 0xABCDEF11
//#define DATA_RATE 1000
//#define RETRANSMITS 4
//
//char postUri[] = "/hum";
//char getConfUri[] = "/conf/hum";
//char postUri2[] = "/temp";
//
//Serial pc(USBTX, USBRX); // tx, rx
//PinName pinA = PinName(A5);
//uint8_t token[] = {1};
//CoapClient coapClient = CoapClient(2, token, RETRANSMITS, DATA_RATE, CHANNEL, RX_ADDRESS, TX_ADDRESS);
//InterruptIn button(USER_BUTTON);
//
//
//// change this
//DHT11 dht(pinA);
//HumidityMeter sensor = HumidityMeter(&dht);
//TempMeter sensor2 = TempMeter(&dht);
//
//int config_interval = 30;
//int sense_interval = 10;
//
//int config_time_left = 0;
//int sense_time_left = 0;
//
//void check_for_new_config() {
//    uint8_t buffer[BUFFER_LEN];
//    int ret = coapClient.get(buffer, BUFFER_LEN, getConfUri);
//    if(ret > 0) {
//        uint8_t sensor_id = 0;
//        int data = -1;
//        Msg::deconstruct_data_msg(&sensor_id, &data, buffer, ret);
//        DBG("INTERVAL: %d", data);
//        sense_interval = data;
//    }
//    DBG("GET: %d", ret);
//}
//
//void collect_and_send_data() {
//    uint8_t buffer[DATA_BUFFER_LEN];
//    int data = sensor.read();
//    DBG("DATA: %d", data);
//    int sensor_id = sensor.getSensorId();
//    // TODO put sensor_id in URI, not in payload
//    Msg::construct_data_msg(sensor_id, data, buffer, DATA_BUFFER_LEN);
//    int ret = coapClient.post(buffer, DATA_BUFFER_LEN, postUri);
//    DBG("POST: %d", ret);
//    
//    data = sensor2.read();
//    DBG("TEMP: %d", data);
//    sensor_id = sensor2.getSensorId();
//    // TODO put sensor_id in URI, not in payload
//    Msg::construct_data_msg(sensor_id, data, buffer, DATA_BUFFER_LEN);
//    ret = coapClient.post(buffer, DATA_BUFFER_LEN, postUri2);
//    DBG("POST: %d", ret);
//}
//
//bool start = false;
//void send_packet() {
//    DBG("Start");
//    start = true;
//}
//
//
//int main() {
//    pc.baud(115200);
//    WakeUp::calibrate();
//    sensor.initialize();
//    sensor2.initialize();
//    DBG("RESTART");
//    int czas_na_spanko = 1;
//    Timer t;
//    button.rise(&send_packet);
//    while(true) {
//        while(start) {
//            if (config_time_left <= 0) {
//                check_for_new_config();
//                config_time_left = config_interval;
//            }
//            if (sense_time_left <= 0) {
//                collect_and_send_data();
//                sense_time_left = sense_interval;
//            }
//
//            czas_na_spanko = min(config_time_left, sense_time_left);
//            DBG("SPANKO: %d", czas_na_spanko);
//            if(czas_na_spanko > 0) {
//                t.stop();
//                //WakeUp::set(czas_na_spanko);
//                wait(czas_na_spanko);
//                t.reset();
//                t.start();
//                config_time_left -= czas_na_spanko;
//                sense_time_left -= czas_na_spanko;
//            } else {
//                config_time_left -= t.read_ms();
//                t.stop();
//                t.reset();
//                t.start();
//            }
//        }
//    }
//}