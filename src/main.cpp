/*
 created Jan 2025
 by Noah Clemmensen
 */
#include <SPI.h>
#include "ApiInterface.h"
#include "IOInterface.h"
#include "secrets.h"
#include <ArduinoJson.h>


char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

char server[] = "192.168.1.100";
int port = 3000;

ApiInterface api(ssid, pass, server, port, true);
IOInterface io(2, DHT11, A3, 3, 4);
ServerSettings settings;

void makePostRequest() {
    Response response = api.Post("/api/temperature/123456789", "{\"temperature\":22}");
    
    if (response.status == 200) {
        if (response.payload.is<JsonObject>()) {
            JsonObject obj = response.payload.as<JsonObject>();
            
            if (obj.containsKey("max_temp")) {
                Serial.println(obj["max_temp"].as<String>());
            }
        } else {
            Serial.println("Payload is not a JSON object");
        }
    } else {
        Serial.print("GET request failed with status: ");
        Serial.println(response.status);
    }
}

void makeGetRequest() {
    Response response = api.Get("/api/settings/123456789");
    
    if (response.status == 200) {
        if (response.payload.is<JsonObject>()) {
            JsonObject obj = response.payload.as<JsonObject>();
            
            if (obj.containsKey("max_temp")) {
                Serial.println(obj["max_temp"].as<String>());
            }
        } else {
            Serial.println("Payload is not a JSON object");
        }
    } else {
        Serial.print("GET request failed with status: ");
        Serial.println(response.status);
    }
}

void setup() {
    //Initialize serial and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    Serial.println("==================================="); // This is just so I can see when a new run starts
    
    if (!api.Initialize()) {
        while (true); // Don't continue if we can't connect to WiFi        
    }
    Response response = api.Get('/api/settings/' + SERIAL);
    settings = {
        max_temp: response.payload['max_temp'],
        min_temp: response.payload['min_temp'],
        max_humidity: response.payload['max_humidity'],
        min_humidity: response.payload['min_humidity'],
        max_sound: response.payload['max_sound'],
        temp_interval: response.payload['temp_interval'],
        humidity_interval: response.payload['humidity_interval']
    };
}

void loop() {
    DHTReading dht = io.ReadDHT();
    int sound = io.ReadSound();

    if (dht.temperature > settings.max_sound || dht.temperature < settings.min_temp) {
        api.Post('/api/temperature' + SERIAL, "{\"temperature\":" + str(dht.temperature) + "}");
    }

    if (dht.humidity > settings.max_humidity || dht.humidity < settings.min_humidity) {
        api.Post('/api/humidity' + SERIAL, "{\"humidity\":" + str(dht.humidity) + "}");
    }

    if (sound > settings.max_sound || sound < settings.max_sound) {
        api.Post('/api/sound' + SERIAL, "{\"sound\":" + str(sound) + "}");
    }

    // Intergrate intervals here
}