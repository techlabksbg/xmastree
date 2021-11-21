#include "webserver.h"
#include "params.h"

#include "WiFi.h"

#include <ESPmDNS.h>
#include <DNSServer.h>


// OTA
#include <WiFiUdp.h>

/*********
 * Inspired by the following project:
 * https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/
 * 
*********/

const char* hostname = "xmastree";  


WebServer::WebServer() {}

void WebServer::setupWiFi(bool ap) {
    if (!ap) {
        WiFi.mode(WIFI_STA);
        Serial.println(WiFi.macAddress());
        WiFi.begin("tech-lab", "tech-lab");
        Serial.print("Connecting to WiFi ");
        int retry = 10;
        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Serial.println("Connection Failed! Retry...");
            delay(2000);
            if (retry--==0) {
                ESP.restart();
            }
        }
        // Print ESP32 Local IP Address
        Serial.println(WiFi.localIP());
    } else {  // Make accessPoint
        WiFi.softAPConfig({192,168,42,1}, {192,168,42,1}, {255,255,255,0});
        WiFi.softAP("passwort_xmastree", "xmastree");
        delay(500);
        Serial.println(WiFi.softAPIP());
    }
}

void WebServer::setupDNS() {
  dnsServer  = new DNSServer();
  // according to https://stackoverflow.com/questions/54583818/esp-auto-login-accept-message-by-os-with-redirect-to-page-like-public-wifi-port
  dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer->start(53, "*", WiFi.softAPIP());

}

void WebServer::setupOTA() {
    ArduinoOTA.setPort(3232);
    ArduinoOTA.setHostname(hostname);
    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else {// U_SPIFFS
        type = "filesystem";
        SPIFFS.end();
      }

      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

void WebServer::setupMDNS() {
    // Set up Hostname
    if(!MDNS.begin(hostname)) {
        Serial.println("Error starting mDNS");
    } else { 
        Serial.print("MDNS started with name ");
        Serial.println(hostname);
        // nicht sicher, ob die folgende Zeile nötig ist, macht aber Sinn...
        // mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
        MDNS.addService("_http", "_tcp", 80);
        MDNS.addService("_dns", "_udp", 53);
    }
}

// Replaces placeholder with LED state value
String processor(const String& var) {
  Serial.println(var);
  if(var == "STATE"){
    return String(params.activeProgram);
  }
  if (var == "BRIGHTNESS") {
    return String(params.brightness);
  }
  if (var == "SPEED") {
    return String(params.speed);
  }
  if (var == "NR") {
    return String(params.activeProgram);
  }
  if (var == "COLOR1") {
    char buf[20];
    sprintf(buf, "%06x", params.color1);
    return String(buf);
  }
  if (var == "COLOR2") {
    char buf[20];
    sprintf(buf, "%06x", params.color2);
    return String(buf);
  }
  if (var == "BUTTONS") {
      String res = "";
      for (int i=0; i<params.apps.size(); i++) {
          res += "<button class=\"button\" onclick=\"state['nr']=";
          res += i;
          res += ";setProgram();\">";
          res += params.apps[i]->buttonName();
          res += "</button>\n";
      }
      return res;
  }
  return String();
}


void WebServer::setupHTTP() {
    // Create AsyncWebServer object on port 80
    server = new AsyncWebServer(80);
  // Route for root / web page
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->host() != (String(hostname) + ".local") && request->host() != String(hostname)  && request->host()!= String("192.168.42.1")) {
      request->redirect("http://192.168.42.1");
    } else { 
      request->send(SPIFFS, "/index.html", String(), false, processor);
    }
  });
  
  // Route to load style.css file
  server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to load hsv400.png file
  server->on("/hsv400.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/hsv400.png", "image/png");
  });

  // Route to load favicon.ico file
  server->on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });

  // Route to set single LEDs
  server->on("/setled", HTTP_GET, [](AsyncWebServerRequest *request){
    String status = String("Status: ");
    if(request->hasParam("led")) {
      int led = atoi(request->getParam("led")->value().c_str());
      params.pixels->clear();
      if (led>=0 && led<NUMPIXEL) {
        params.pixels->setPixelColor(led, 255,255,255);
        status += " Pixel ";
        status += led;
        status += "set.";
      } else {
        status += " all clear.";
      }
      params.pixels->show();
      if (params.activeProgram!=-1) {
        params.newProgram = -1;
        params.activeProgram = -1;
      }
    } else {
      status += "missing parameter 'led'";
    }
    params.lastCmd = millis();
    params.isAutoRunning = false;
    request->send(200, "text/plain", status);
  });

  // Route to set Programms
  server->on("/prg", HTTP_GET, [](AsyncWebServerRequest *request){
    params.lastCmd = millis();
    params.isAutoRunning = false;

    String status = String("Status: ");
    if(request->hasParam("brightness")) {
      int b = atoi(request->getParam("brightness")->value().c_str());
      b = (b<0 ? 0 : (b>255 ? 255 : b));
      if (b!=params.brightness) {
        params.brightness = b;
        params.pixels->setBrightness(params.brightness);
        Serial.printf("Brightness=%d\n", params.brightness);
        status += "Brightness=";
        status += params.brightness;
        status += " &nbsp; ";
      }
    }
    if(request->hasParam("speed")) {
      int s = atoi(request->getParam("speed")->value().c_str());
      s = (s<0 ? 0 : (s>255 ? 255 : s));
      if (s!=params.speed) {
        params.speed = s;
        Serial.printf("Speed=%d\n", params.speed);
        status+="speed=";
        status+=params.speed;
        status+=" &nbsp; ";
      }
    }
    if(request->hasParam("color1")) {      
      int c1 = strtol(request->getParam("color1")->value().c_str(), NULL, 16);
      if (c1!=params.color1) {
        params.color1=c1;
        Serial.printf("Color1=%06x\n", params.color1);
        char buf[20];
        sprintf(buf, "%06x", params.color1);        
        status+="color1=";
        status+=buf;
        status+=" &nbsp; ";
      }
    }
    if(request->hasParam("color2")) {
      int c2 = strtol(request->getParam("color2")->value().c_str(), NULL, 16);
      if (c2!=params.color2) {
        params.color2=c2;
        Serial.printf("Color2=%06x\n", params.color2);
        char buf[20];
        sprintf(buf, "%06x", params.color2);        
        status+="color2=";
        status+=buf;
        status+=" &nbsp; ";
      }
    }
    if(request->hasParam("nr")) {
      int p  = atoi(request->getParam("nr")->value().c_str());
      p =  (p<0) ? 0 : (p>=params.numPrograms ? params.numPrograms-1 : p);
      if (p!=params.activeProgram) {
        params.newProgram = p;
        status += " program=";
        status += params.newProgram;
      }
    }
    request->send(200, "text/plain", status);
  });
 
  // Start server
  server->begin();
  Serial.println("WebServer up and running!");


}

void WebServer::begin(bool accessPoint) {
    setupWiFi(accessPoint);
    setupOTA();
    setupHTTP();
    setupMDNS();
    setupDNS();
  }

  void WebServer::loop() {
    dnsServer->processNextRequest();
    ArduinoOTA.handle();
  }