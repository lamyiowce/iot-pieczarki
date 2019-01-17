#include "sensor.h"
#include "distancemeter.h"
#include "mbed.h"
#include "msg.h"

// https://os.mbed.com/users/Sissors/code/WakeUp/
#include "WakeUp.h"

#define BUFFER_LEN 16

PinName pinA = PinName(...);
PinName pinB = PinName(...);
CoapClient coapClient = CoapClient(...);

// TODO #ifdef
Sensor sensor = DistanceMeter(pinA, pinB);

int default_time_to_check = 600;
int default_time_to_collect = 60;

int time_to_check_for_new_config = 0;
int time_to_collect_and_send_data = 0;

void check_for_new_config() {
    uint8_t buffer[BUFFER_LEN];
    coapClient.get(buffer, BUFFER_LEN, ...);
    // TODO update default_time_to_collect if necessary
}

void collect_and_send_data() {
    uint8_t buffer[BUFFER_LEN];
    int data = sensor.read();
    int sensor_id = sensor.getSensorId();
    // TODO put sensor_id in URI, not in payload
    Msg::construct_data_msg(sensor_id, data, buffer, BUFFER_LEN);
    coapClient.post(buffer, BUFFER_LEN, ...);
}

int main() {
    WakeUp::calibrate();
    sensor.initialize();
    coapClient.initialize();

    int czas_na_spanko;

    while(true) {
        if (time_to_check_for_new_config == 0) {
            check_for_new_config();
            time_to_check_for_new_config = default_time_to_check;
        }
        if (time_to_collect_and_send_data == 0) {
            collect_and_send_data();
            time_to_collect_and_send_data = default_time_to_collect;
        }
        
        czas_na_spanko = min(time_to_check_for_new_config, time_to_collect_and_send_data);
        WakeUp::set(czas_na_spanko);
        deepsleep();
        
        time_to_check_for_new_config -= czas_na_spanko;
        time_to_collect_and_send_data-= czas_na_spanko;

    }
}
