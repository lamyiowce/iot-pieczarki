#ifndef SLAVE_MOISTUREMETER_H
#define SLAVE_MOISTUREMETER_H

#include "sensor.h"

class MoistureMeter : public Sensor {

private:
    AnalogIn analogIn;

public:
    MoistureMeter(PinName dataPin) : analogIn(dataPin) {}
    
    int read();
    
    int getSensorId();

    void initialize();
};

#endif //SLAVE_MOISTUREMETER_H