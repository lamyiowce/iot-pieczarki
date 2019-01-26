//
// Created by Kamil Mykitiuk on 2019-01-21.
//

#ifndef SLAVE_HUMIDITYMETER_H
#define SLAVE_HUMIDITYMETER_H

#include "DHT11.h"
#include "sensor.h"

class HumidityMeter : public Sensor {

private:
    DHT11* dht;


public:
    HumidityMeter(DHT11* dht) : dht(dht) {}
    
    int read();
    
    int getSensorId();

    void initialize();
};



#endif //SLAVE_HUMIDITYMETER_H
