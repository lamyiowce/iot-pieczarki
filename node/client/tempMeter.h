//
// Created by Kamil Mykitiuk on 2019-01-21.
//

#ifndef SLAVE_TEMPMETER_H
#define SLAVE_TEMPMETER_H

#include "DHT11.h"
#include "sensor.h"

class TempMeter : public Sensor {

private:
    DHT11* dht;


public:
    TempMeter(DHT11* dht) : dht(dht) {}
    
    int read();
    
    int getSensorId();

    void initialize();
};



#endif //SLAVE_TEMPMETER_H
