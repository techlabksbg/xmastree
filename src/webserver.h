#pragma once

#include "ESPAsyncWebServer.h"
#include <ArduinoOTA.h>



class WebServer {
public: 
    WebServer();
    void begin(bool accessPoint=false);

private:

    void setupWiFi(bool ap);
    void setupOTA();
    void setupHTTP();
    void setupMDNS();

    AsyncWebServer* server;

};
