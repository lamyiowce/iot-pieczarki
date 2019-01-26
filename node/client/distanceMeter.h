#ifndef MBEDDED_DISTANCEMETER_H
#define MBEDDED_DISTANCEMETER_H

#include "sensor.h"

class DistanceMeter : public Sensor {
private:
    InterruptIn echo;
    DigitalOut trigger;

    Timer timer;
    int timeUs;
    bool timeReady;

    void echoUp();
    void echoDown();

public:
    int read();
    
    int getSensorId();

    void initialize();
    
    DistanceMeter(PinName echoPin, PinName triggerPin);
};

#endif //MBEDDED_DISTANCEMETER_H
