#include "msg.h"

#define UINT8_MAX 256

void Msg::construct_data_msg(uint8_t sensor_id, int data, uint8_t *buffer, int buffer_len) {
    if (buffer_len < 5)
        return;

    buffer[0] = sensor_id;
    for (int i = 4; i > 0; i--) {
        buffer[i] = data % UINT8_MAX;
        data /= UINT8_MAX;
    }
}

void Msg::deconstruct_data_msg(uint8_t *sensor_id, int *data, uint8_t *buffer, int buffer_len) {
    if (buffer_len < 5)
        return;

    *sensor_id = buffer[0];
    *data = 0;
    for (int i = 1; i < 5; i++) {
        *data = *data * UINT8_MAX + buffer[i];
    }
}
