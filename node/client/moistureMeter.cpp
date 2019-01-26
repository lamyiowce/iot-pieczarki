#include "moistureMeter.h"
#include "dbg.h"

int MoistureMeter::read() {
    DBG("Reading moisture...\n\r");
    int readValue = (int)1000*analogIn.read();
    DBG("Moisture read: %d\n\r", readValue);
    return readValue;
}

int MoistureMeter::getSensorId() {
    return 2;
}

void MoistureMeter::initialize() {
    DBG("Moisture sensor initialized");
}