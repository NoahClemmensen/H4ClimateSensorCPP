// IOInterface.h
#include "api/Common.h"
#ifndef IO_INTERFACE_H
#define IO_INTERFACE_H

#include "Grove_Temperature_And_Humidity_Sensor.h"

struct DHTReading {
    float temperature;
    float humidity;
};

/// @brief Uses the WiFiNINA library to simplify making GET and POST requests
class IOInterface {
    public:
        /* constructor */
        IOInterface(int dhtPin, int dhtType, int sound, int green, int red);

        DHTReading ReadDHT();
        int ReadSound();
        void SetGreenLed(bool on);
        void SetRedLed(bool on);
    private:
        DHT dht;
        int soundPin;
        int greenLedPin;
        int redLedPin;
};

#endif