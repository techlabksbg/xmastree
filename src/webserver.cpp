#include "webserver.h"
#include "params.h"

#include "WiFi.h"

#include <ESPmDNS.h>
#include <DNSServer.h>

#if __has_include("secrets.h")
#include "secrets.h"
#else 
// Define your secrets in this manner in secrets.h
#define WIFI_SSID "tech-lab"
#define WIFI_PASS "tech-lab"
#endif

// OTA
#include <WiFiUdp.h>

/*********
 * Inspired by the following project:
 * https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/
 * 
*********/

const char* hostname = "ledwall";


WebServer::WebServer() {}

bool ntpDone = false;

void getTimeFromNTP() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("St.Galler Wireless", "");
  Serial.println("Trying St.Galler Wireless");
  int i = 0;
  while (i<50 && (WiFi.status()!= WL_CONNECTED)) {
    delay(100);
    i++;
    Serial.print('.');
  }
  Serial.println("Done trying");
  if (WiFi.status()== WL_CONNECTED) {
    Serial.println("Syncing time with NTP");
    sntp_set_time_sync_notification_cb([](timeval *tv) {
        ntpDone = true;
    });
    configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org");
    i = 0;
    while (i<10 && !ntpDone) {
      delay(100);
      i++;
      Serial.print('.');
    }
    Serial.println();
  } else {
    Serial.println("St.Galler Wireless not found");
  }
  Serial.println("WiFi disconnect");
  WiFi.disconnect();
}

void WebServer::setupWiFi() {
    if (!isAp) { // connect to TEch-Lab network
        WiFi.mode(WIFI_STA);
        Serial.println(WiFi.macAddress());
        WiFi.begin(WIFI_SSID, WIFI_PASS);
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
        // Daylightsaving time included
        configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org");
    } else {  // Make accessPoint
        WiFi.softAPConfig({192,168,42,1}, {192,168,42,1}, {255,255,255,0});
        // Get time first
        getTimeFromNTP();
        WiFi.softAPConfig({192,168,42,1}, {192,168,42,1}, {255,255,255,0});
        // make AP
        WiFi.softAP("passwort_xmastree", "xmastree");
        delay(500);
        Serial.println(WiFi.softAPIP());
    }
}
/*
void WebServer::setupDNS() {
  dnsServer  = new DNSServer();
  // according to https://stackoverflow.com/questions/54583818/esp-auto-login-accept-message-by-os-with-redirect-to-page-like-public-wifi-port
  dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer->start(53, "*", WiFi.softAPIP());

}*/
/*
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
}*/

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
        if (isAp) {
          MDNS.addService("_dns", "_udp", 53);
        }
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
    sprintf(buf, "%06x", HtmlColor(params.color1).Color);
    return String(buf);
  }
  if (var == "COLOR2") {
    char buf[20];
    sprintf(buf, "%06x", HtmlColor(params.color2).Color);
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
  if (var == "TEXT") {
    return params.text;
  }
  return String();
}


void WebServer::setupHTTP() {
    // Create AsyncWebServer object on port 80
    server = new AsyncWebServer(80);
  // Route for root / web page
  bool makeRedirect = isAp;
  server->on("/", HTTP_GET, [makeRedirect](AsyncWebServerRequest *request){
    if (makeRedirect && request->host() != (String(hostname) + ".local") && request->host() != String(hostname)  && request->host()!= String("192.168.42.1")) {
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

  // Route to load techlab.png file
  server->on("/techlab.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/techlab.png", "image/png");
  });

  // Route to load treebg.png file
  server->on("/treebg.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/treebg.png", "image/png");
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
      params.pixels->ClearTo(RgbColor(0,0,0));
      if (led>=0 && led<NUMPIXEL) {
        params.pixels->SetPixelColor(led, RgbColor(255,255,255));
        status += " Pixel ";
        status += led;
        status += "set.";
        params.singlePixel = led;
      } else {
        params.singlePixel = -1;
        status += " all clear.";
      }
      params.pixels->Show();
      if (params.activeProgram!=-1) {
        params.newProgram = -1;
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
        //params.pixels->setBrightness(params.brightness);
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
      RgbColor c1 = RgbColor(HtmlColor(strtol(request->getParam("color1")->value().c_str(),0,16)));
      if (c1!=params.color1) {
        params.color1=c1;
        Serial.printf("Color1=%06x\n", HtmlColor(params.color1).Color);
        char buf[20];
        sprintf(buf, "%06x", HtmlColor(params.color1).Color);        
        status+="color1=";
        status+=buf;
        status+=" &nbsp; ";
      }
    }
    if(request->hasParam("color2")) {
      RgbColor c2 = RgbColor(HtmlColor(strtol(request->getParam("color2")->value().c_str(),0,16)));
      if (c2!=params.color2) {
        params.color2=c2;
        Serial.printf("Color2=%06x\n", HtmlColor(params.color2).Color);
        char buf[20];
        sprintf(buf, "%06x", HtmlColor(params.color2).Color);        
        status+="color2=";
        status+=buf;
        status+=" &nbsp; ";
      }
    }
    if(request->hasParam("nr")) {
      int p  = atoi(request->getParam("nr")->value().c_str());
      p =  (p<0) ? 0 : (p>=params.apps.capacity() ? params.apps.capacity()-1 : p);
      if (p!=params.activeProgram) {
        params.newProgram = p;
        status += " program=";
        status += params.newProgram;
      }
    }
    if(request->hasParam("drawx")) {
      params.drawx = request->getParam("drawx")->value().toInt();
      if(request->hasParam("drawx")) {
        params.drawy = request->getParam("drawy")->value().toInt();
      }
      int drawId = params.getAppId("Drawing");
      if (drawId>=0) {
        if (params.drawx || params.drawy) {
          if (drawId>=0) {
            params.newProgram = drawId;
            status += params.drawx;
            status += ", ";
            status += params.drawy;
          }
        }
      }
    }
    if (request->hasParam("text")) {
      if (params.text != request->getParam("text")->value()) {
        params.text = request->getParam("text")->value();
        status += "text=";
        status += params.text;
      }
    }
    request->send(200, "text/plain", status);
  });
 
  // File Upload stuff
    server->on("/sdcard/", HTTP_GET, [](AsyncWebServerRequest *request) {
        // From https://platformio.org/lib/show/6758/ESPAsyncWebServer-esphome/examples
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->print("<!DOCTYPE html><html><head><title>WebFiles</title></head><body>");
        response->print("<form action=\"/sdcard/upload\" method=\"post\" enctype=\"multipart/form-data\"><input type=\"file\" id=\"myfile\" name=\"myfile\"><input type=\"submit\"></form><br>");
        // from https://techtutorialsx.com/2019/02/23/esp32-arduino-list-all-files-in-the-spiffs-file-system/
        fs::File root = SD.open("/vids");
        fs::File file = root.openNextFile();
        while(file) {
            response->print("<li><a href=\"/sdcard");
            response->print(file.name());
            response->print("\">");
            response->print(file.name());
            // TODO: FIXME: correctly url-encode file names.
            response->print("</a> &nbsp; <a href=\"/sdcard/delete?filename=");
            response->print(file.name());
            response->print("\">delete</a>"); 
            response->print("</li>\n");
            Serial.println(file.name());        
            file = root.openNextFile();
        }

        response->print("</ul></body></html>\n");
        request->send(response);
    });

    server->on("/sdcard/upload", HTTP_POST, [](AsyncWebServerRequest *request) {}, 
    [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        static fs::File file;
        if (!index) {
            Serial.print("Upoad URL = ");
            Serial.println(request->url());
            file = SD.open(String("/vids/") + filename, FILE_WRITE);
            if (!file) {
                Serial.print("Could not open file ");
                Serial.println(filename);
            }
            Serial.printf("UploadStart: %s\n", filename.c_str());
        }
        file.write(data, len);
        if (final) {
            if (file) {
                file.close();
                request->redirect("/sdcard/");
                //request->send(200, "text/plain", "Upload complete");
            } else {
                request->redirect("/");
                //request->send(200, "text/plain", "Upload FAILED!");
            }
            Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index + len);
        }
    });


    
    server->on("/sdcard/delete", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("on /delete");
        if(request->hasParam("filename")) {
            // TODO: FIXME: correctly decode url-encoded file names
            Serial.println(request->getParam("filename")->value());
            SD.remove(request->getParam("filename")->value());
        }
        request->redirect("/sdcard/");
    }); 


    server->on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){
        ESP.restart();
    }); 

  server->serveStatic("/sdcard/vids", SD, "/vids");
  // Start server
  server->begin();
  Serial.println("WebServer up and running!");


}

void WebServer::begin(bool accessPoint) {
    isAp = accessPoint;
    setupWiFi();
    //setupOTA();
    setupHTTP();
    setupMDNS();
    if (isAp) {
      //setupDNS();
    }
  }

  void WebServer::loop() {
    //dnsServer->processNextRequest();
    //ArduinoOTA.handle();
  }