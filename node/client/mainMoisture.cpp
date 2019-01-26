//#include "sensor.h"
//#include "humidityMeter.h"
//#include "moistureMeter.h"
//#include "mbed.h"
//#include "msg.h"
//#include "dbg.h"
//#include "coapClient.h"
//#include <string>
//
//// https://os.mbed.com/users/Sissors/code/WakeUp/
//#include "WakeUp.h"
//
//#define BUFFER_LEN 32
//#define CHANNEL 4
//#define RX_ADDRESS 0xABCDEF08
//#define TX_ADDRESS 0xABCDEF12
//#define DATA_RATE 1000
//#define RETRANSMITS 4
//
//char postUri[] = "/moist";
//char getConfUri[] = "/conf/moist";
//
//Serial pc(USBTX, USBRX); // tx, rx
//PinName pinA = PinName(PC_0);
//uint8_t token[] = {3};
//CoapClient coapClient = CoapClient(3,token, RETRANSMITS, DATA_RATE, CHANNEL, RX_ADDRESS, TX_ADDRESS);
//InterruptIn button(USER_BUTTON);
//
//
//// TODO #ifdef
//MoistureMeter sensor = MoistureMeter(pinA);
//
//int config_interval = 30;
//int sense_interval = 10;
//
//int config_time_left = 0;
//int sense_time_left = 0;
//
//void check_for_new_config() {
//    DBG("************ ASKING FOR NEW CONFIG...");
//    uint8_t buffer[BUFFER_LEN];
//    uint8_t sensor_id;
//    int new_interval;
//    int ret = coapClient.get(buffer, BUFFER_LEN, getConfUri);
//    if(ret > 0) {
//        Msg::deconstruct_data_msg(&sensor_id, &new_interval, buffer, BUFFER_LEN);
//        sense_interval = new_interval;
//        DBG("************ NEW INTERVAL IS %d.", sense_interval);
//    }
//    DBG("************ GET RET: %d", ret);
//}
//
//void collect_and_send_data() {
//    uint8_t buffer[10];
//    int data = sensor.read();
//    int sensor_id = sensor.getSensorId();
//    // TODO put sensor_id in URI, not in payload
//    DBG("************ SENDING VALUE %d...", data);
//    Msg::construct_data_msg(sensor_id, data, buffer, 10);
//    int ret = coapClient.post(buffer, 10, postUri);
//    DBG("************ POST RET: %d", ret);
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
//            DBG("*********** Config time left %d.", config_time_left);
//            DBG("*********** Sense time left %d.", sense_time_left);
//            DBG("*********** Interval %d.", sense_interval);
//            DBG("*********** Czas na spanko to %d.", czas_na_spanko);
//            if(czas_na_spanko > 0) {
//                t.stop();
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