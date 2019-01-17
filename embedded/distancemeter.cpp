#include "distancemeter.h"

void DistanceMeter::echoUp() {
    timer.start();
}

void DistanceMeter::echoDown() {
    timer.stop();
    timeUs = timer.read_us();
    timer.reset();
    timeReady = true;
}

DistanceMeter::DistanceMeter(PinName echoPin, PinName triggerPin) {
    echo = InterruptIn(echoPin);
    trigger = DigitalOut(triggerPin);
    trigger = 0;
}

void DistanceMeter::initialize() {
    echo.rise(&echoUp);
    echo.fall(&echoDown);
}

int DistanceMeter::read() {
    trigger = 1;
    wait_us(10);
    trigger = 0;

    while(!timeReady)
        wait_us(100);

    timeReady = false;
    return timeUs * 17 / 1000; // dist in cm, (time * 340) / (2 * 10000)

}

int DistanceMeter::getSensorId() {
    return 0;
}