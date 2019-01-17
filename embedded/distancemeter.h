#ifndef MBEDDED_DISTANCEMETER_H
#define MBEDDED_DISTANCEMETER_H

#include "sensor.h"

class DistanceMeter : public Sensor {
private:
    InterruptIn echo;
    DigitalOut trigger;

    Timer timer;
    int timeUs;
    bool timeReady = false;

    echoUp();
    echoDown();

public:
    DistanceMeter(PinName echoPin, PinName triggerPin);
};

#endif //MBEDDED_DISTANCEMETER_H
