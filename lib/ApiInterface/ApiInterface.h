// ApiInterface.h
#include "api/Common.h"
#ifndef API_INTERFACE_H
#define API_INTERFACE_H

#include <WiFiNINA.h>
#include <ArduinoJson.h>

struct Response {
    int status;
    String headers;
    DynamicJsonDocument payload;

    Response(): payload(1024) {}
};

struct ServerSettings {
  float max_temp;
  float min_temp;
  float max_humidity;
  float min_humidity;
  float max_sound;
  float temp_interval;
  float humidity_interval;
};

/// @brief Uses the WiFiNINA library to simplify making GET and POST requests
class ApiInterface {
  public:
    /* constructor */
    ApiInterface(String ssid, String pass, String server, int port, bool verbose = false);

    bool Initialize(int maxTries = 10, int retryDelay = 10000, bool logWiFiData = true);

    Response Get(String url);
    Response Post(String url, String json);

    void Update();

    /* Destructor */
    ~ApiInterface();

  private:
    WiFiClient client;
    int port;
    int status;
    bool verbose;
    String ssid;
    String pass;
    String server;

    void printWifiData();
    void printCurrentNet();
    void printMacAddress(byte mac[]);
    void connectToWiFi();
    void printWifiStatuses();
    Response readResponse();
    String statusToString(int status);
};

#endif