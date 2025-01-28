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

    makeGetRequest();
    delay(5000);
    makePostRequest();
}

void loop() {
    //api.Update();
    //Serial.println(io.ReadSound());
}