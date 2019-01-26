#ifndef MBEDDED_MSG_H
#define MBEDDED_MSG_H

#include<stdint.h>

class Msg {
public:

    static void construct_data_msg(uint8_t sensor_id, int data, uint8_t buffer[], int buffer_len);

    static void deconstruct_data_msg(uint8_t* sensor_id, int* data, uint8_t buffer[], int buffer_len);

    static void construct_config_msg(uint8_t sensor_id, int interval, uint8_t buffer[], int buffer_len);

    static void deconstruct_config_msg(uint8_t sensor_id, int interval, uint8_t buffer[], int buffer_len);
};

#endif //MBEDDED_MSG_H
