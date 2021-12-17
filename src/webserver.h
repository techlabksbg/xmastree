#pragma once

#include "ESPAsyncWebServer.h"
#include <ArduinoOTA.h>
#include "DNSServer.h"



class WebServer {
public: 
    WebServer();
    void begin(bool accessPoint=false);
    void loop();

private:

    void setupWiFi();
    void setupOTA();
    void setupHTTP();
    void setupMDNS();
    void setupDNS();

    AsyncWebServer* server;
    DNSServer * dnsServer;
    bool isAp = false;

};
