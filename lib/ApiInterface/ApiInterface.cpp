#include "ApiInterface.h"
#include <WiFiNINA.h>
#include <ArduinoJson.h>

// @brief Constructor. Sets the SSID, password, server, and port
ApiInterface::ApiInterface(String ssid, String pass, String server, int port, bool verbose = false) {
    status = WL_IDLE_STATUS;
    this->verbose = verbose;
    this->ssid = ssid;
    this->pass = pass;
    this->server = server;
    this->port = port;
}

// @brief Destructor. Stops the client and ends the WiFi connection
ApiInterface::~ApiInterface() {
  client.stop();
  WiFi.end();
}

// @brief Initialize the WiFi module and connect to the network
bool ApiInterface::Initialize(int maxTries = 10, int retryDelay = 10000, bool logWiFiData = true) {
    // Check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        return false;
    }

    // Check for the firmware
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
        return false;
    }

    // Attempt to connect to WiFi network:
    int tries = 0;
    bool connected = false;
    while (status != WL_CONNECTED && tries <= maxTries) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid.c_str());
        // Connect to network:
        status = WiFi.begin(ssid.c_str(), pass.c_str());

        if (verbose) Serial.println(statusToString(status));
        if (status == WL_CONNECTED) {
        connected = true;
        }

        tries++;
        // wait 10 (by default) seconds for connection:
        delay(retryDelay);
    }

    if (connected) {
        Serial.println("You're connected to the network");
        if (logWiFiData) {
            if (verbose) printCurrentNet();
            if (verbose) printWifiData();
        }

        return true;
    } else {
        Serial.println("Max tries reached");
        return false;
    }
}

// @brief Make a GET request to the server
Response ApiInterface::Get(String url) {
    Response response;
    response.status = -1; // Default to an error status

    // If we are connected to the WiFi network
    if (WiFi.status() == WL_CONNECTED) {
        // If there already is a connection
        if (client.status() == WL_CONNECTED) {
            client.println("GET " + url + " HTTP/1.1");
            client.println("Host: " + server);
            client.println("Connection: close");
            client.println(); // end HTTP request header
        } else { // Make a new connection
           if (client.connect(server.c_str(), port)) {
                client.println("GET " + url + " HTTP/1.1");
                client.println("Host: " + server);
                client.println("Connection: close");
                client.println(); // end HTTP request header
            } else {
                return response;
            } 
        }
        response = readResponse();
    }

    return response;
}

// @brief Make a POST request to the server
Response ApiInterface::Post(String url, String json) {
    Response response;
    response.status = -1; // Default to an error status

    // If we are connected to the WiFi network
    if (WiFi.status() == WL_CONNECTED) {
        // If there already is a connection
        if (client.status() == WL_CONNECTED) {
            client.println("POST " + url + " HTTP/1.1");
            client.println("Host: " + server);
            client.println("Content-Type: application/json");
            client.println("Content-Length: " + String(json.length()));
            client.println("Connection: close");
            client.println();
            client.println(json);
        } else { // Make a new connection
           if (client.connect(server.c_str(), port)) {
                client.println("POST " + url + " HTTP/1.1");
                client.println("Host: " + server);
                client.println("Content-Type: application/json");
                client.println("Content-Length: " + String(json.length()));
                client.println("Connection: close");
                client.println();
                client.println(json);
            } else {
                return response;
            } 
        }
        response = readResponse();
    }

    return response;
}

/* == Private methods == */

// @brief Read the response from the server
Response ApiInterface::readResponse() {
    Response response;
    response.status = -1;
    response.headers = "";
    String payloadStr;

    boolean headersEnded = false;
    String line;
    while (client.connected() || client.available()) {
        line = client.readStringUntil('\n');
        if (line == "\r") {
            headersEnded = true;
        } else if (!headersEnded) {
            response.headers += line + "\n";
            if (line.startsWith("HTTP/1.1")) {
                response.status = line.substring(9, 12).toInt();
            }
        } else {
            payloadStr += line + "\n";
        }
    }

    // Parse the JSON payload
    DeserializationError error = deserializeJson(response.payload, payloadStr);
    if (error) {
        response.status = -1; // Indicate an error in parsing JSON
    }

    return response;
}

// @brief Print the WiFi data
void ApiInterface::printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

// @brief Print the current network data
void ApiInterface::printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

// @brief Print the MAC address
void ApiInterface::printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void ApiInterface::printWifiStatuses() {
    Serial.print("WL_CONNECTED: ");
    Serial.println(WL_CONNECTED);

    Serial.print("WL_AP_CONNECTED: ");
    Serial.println(WL_AP_CONNECTED);

    Serial.print("WL_AP_LISTENING: ");
    Serial.println(WL_AP_LISTENING);

    Serial.print("WL_NO_SHIELD: ");
    Serial.println(WL_NO_SHIELD);

    Serial.print("WL_NO_MODULE: ");
    Serial.println(WL_NO_MODULE);

    Serial.print("WL_IDLE_STATUS: ");
    Serial.println(WL_IDLE_STATUS);

    Serial.print("WL_NO_SSID_AVAIL: ");
    Serial.println(WL_NO_SSID_AVAIL);

    Serial.print("WL_SCAN_COMPLETED: ");
    Serial.println(WL_SCAN_COMPLETED);

    Serial.print("WL_CONNECT_FAILED: ");
    Serial.println(WL_CONNECT_FAILED);

    Serial.print("WL_CONNECTION_LOST: ");
    Serial.println(WL_CONNECTION_LOST);

    Serial.print("WL_DISCONNECTED: ");
    Serial.println(WL_DISCONNECTED);
}

String ApiInterface::statusToString(int status) {
    switch (status) {
        case WL_CONNECTED:
            return "Connected";
        case WL_AP_CONNECTED:
            return "AP Connected";
        case WL_AP_LISTENING:
            return "AP Listening";
        case 255:
            return "No Shield/Module";
        case WL_IDLE_STATUS:
            return "Idle";
        case WL_NO_SSID_AVAIL:
            return "No SSID Available";
        case WL_SCAN_COMPLETED:
            return "Scan Completed";
        case WL_CONNECT_FAILED:
            return "Connect Failed";
        case WL_CONNECTION_LOST:
            return "Connection Lost";
        case WL_DISCONNECTED:
            return "Disconnected";
        default:
            return "Unknown";
    }
}
