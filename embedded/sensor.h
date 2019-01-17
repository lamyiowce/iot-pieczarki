#ifndef MBEDDED_SENSOR_H
#define MBEDDED_SENSOR_H

#include "mbed.h"

class Sensor {
public:
    virtual void initialize() = 0;

    virtual int read() = 0;

    virtual int getSensorId() = 0; // different ID for each type of sensor
};


#endif //MBEDDED_SENSOR_H
