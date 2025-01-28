#include "IOInterface.h"
#include "Grove_Temperature_And_Humidity_Sensor.h"

IOInterface::IOInterface(int dhtPin, int dhtType, int sound, int green, int red)
    : dht(dhtPin, dhtType) {
    soundPin = sound;
    greenLedPin = green;
    redLedPin = red;

    Wire.begin();
    dht.begin();

    pinMode(soundPin, INPUT);
    pinMode(greenLedPin, OUTPUT);
    pinMode(redLedPin, OUTPUT);
}

DHTReading IOInterface::ReadDHT() {
    DHTReading reading;
    reading.temperature = 0;
    reading.humidity = 0;

    float temp_hum_val[2] = {0};

    if (!dht.readTempAndHumidity(temp_hum_val)) {
        reading.temperature = temp_hum_val[1];
        reading.humidity = temp_hum_val[0];
    } else {
        Serial.println("Failed to get temprature and humidity value.");
    }

    return reading;
}

int IOInterface::ReadSound() {
    long sum = 0;
    for(int i=0; i<32; i++)
    {
        sum += analogRead(soundPin);
    }

    sum >>= 5;

    return sum;
}

void IOInterface::SetGreenLed(bool on) {
    digitalWrite(greenLedPin, on ? HIGH : LOW);
}

void IOInterface::SetRedLed(bool on) {
    digitalWrite(redLedPin, on ? HIGH : LOW);
}