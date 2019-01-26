//
// Created by Kamil Mykitiuk on 2019-01-21.
//

#include "humidityMeter.h"
#include "dbg.h"

int HumidityMeter::read() {
    int s = dht->readData();
    if (s != DHT11::OK) {
        return s * -1;
    } else {
        return dht->readHumidity();
    }

}

int HumidityMeter::getSensorId() {
    return 1;
}

void HumidityMeter::initialize() {
    DBG("Sensor initialized");
}