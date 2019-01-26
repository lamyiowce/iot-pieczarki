//
// Created by Kamil Mykitiuk on 2019-01-21.
//

#include "tempMeter.h"
#include "dbg.h"

int TempMeter::read() {
    //int s = dht->readData();
    //if (s != DHT11::OK) {
    //    return s * -1;
    //} else {
        return dht->readTemperature();
   // }

}

int TempMeter::getSensorId() {
    return 3;
}

void TempMeter::initialize() {
    DBG("Sensor initialized");
}