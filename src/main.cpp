  /*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
  https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/

  Zusätzliche Libraries für diesen Sketch:
  https://github.com/me-no-dev/AsyncTCP/archive/master.zip
  https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip

  Entpacken, umbenennen (ohne -master) und nach ~/Arduino/libraries/. speichern.

  Zusätzliche Tools für diesen Sketch:
  https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/tag/1.0
  https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/download/1.0/ESP32FS-1.0.zip
  In den Ordner ~/Arudino/tools/. entpacken (den Ordner tools anlegen, falls nötig).
  
*********/

// Import required libraries
#include <Arduino.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <ESPmDNS.h>
//#include <esp_wifi.h>  // To set MAC-Adress (to avoid new MAC-Adresses every time).


// OTA
#include <WiFiUdp.h>
#include <ArduinoOTA.h>



// Replace with your network credentials
const char* ssid = "tech-lab";
const char* password = "tech-lab";
const char* hostname = "xmas";  
//uint8_t myMACAddress[] = {0x3c,0x71,0xbf,0xfb,0x9d,0x00};

#include <Adafruit_NeoPixel.h>
 
#define PIN 13
#define NUMPIXEL 200

//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXEL, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXEL, PIN, NEO_RGB + NEO_KHZ400);


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

int activeProgram = 0;
int newProgram = 2;
int numPrograms = 4;
int brightness = 255;
int speed = 35;
uint32_t color1 = 0xff0000;
uint32_t color2 = 0x0000ff;



// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    return String(activeProgram);
  }
  if (var == "BRIGHTNESS") {
    return String(brightness);
  }
  if (var == "SPEED") {
    return String(speed);
  }
  if (var == "NR") {
    return String(activeProgram);
  }
  if (var == "COLOR1") {
    char buf[20];
    sprintf(buf, "%06x", color1);
    return String(buf);
  }
  if (var == "COLOR2") {
    char buf[20];
    sprintf(buf, "%06x", color2);
    return String(buf);
  }
  return String();
}

 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  //3c:71:bf:fb:9d:00
//  esp_wifi_set_mac(ESP_IF_WIFI_STA, &myMACAddress[0]);
  //esp_wifi_set_mac(ESP_IF_WIFI_STA, myMACAddress);
  Serial.println(WiFi.macAddress());
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");
  Serial.println(ssid);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(2000);
    ESP.restart();
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

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

  // Set up Hostname
  if(!MDNS.begin(hostname)) {
     Serial.println("Error starting mDNS");
  } else { 
    Serial.print("MDNS started with name ");
    Serial.println(hostname);
    // nicht sicher, ob die folgende Zeile nötig ist, macht aber Sinn...
//    mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
    MDNS.addService("_http", "_tcp", 80);
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to load hsv400.png file
  server.on("/hsv400.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/hsv400.png", "image/png");
  });

  // Route to load favicon.ico file
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });

  // Route to set single LEDs
  server.on("/setled", HTTP_GET, [](AsyncWebServerRequest *request){
    String status = String("Status: ");
    if(request->hasParam("led")) {
      int led = atoi(request->getParam("led")->value().c_str());
      pixels.clear();
      if (led>=0 && led<NUMPIXEL) {
        pixels.setPixelColor(led, 255,255,255);
        status += " Pixel ";
        status += led;
        status += "set.";
      } else {
        status += " all clear.";
      }
      pixels.show();
      if (activeProgram!=4) {
        newProgram = 4;
        activeProgram = 4;
      }
    } else {
      status += "missing parameter 'led'";
    }
    request->send(200, "text/plain", status);
  });

  // Route to set Programms
  server.on("/prg", HTTP_GET, [](AsyncWebServerRequest *request){
    String status = String("Status: ");
    if(request->hasParam("brightness")) {
      int b = atoi(request->getParam("brightness")->value().c_str());
      b = (b<0 ? 0 : (b>255 ? 255 : b));
      if (b!=brightness) {
        brightness = b;
        pixels.setBrightness(brightness);
        Serial.printf("Brightness=%d\n", brightness);
        status += "Brightness=";
        status += brightness;
        status += " &nbsp; ";
      }
    }
    if(request->hasParam("speed")) {
      int s = atoi(request->getParam("speed")->value().c_str());
      s = (s<0 ? 0 : (s>255 ? 255 : s));
      if (s!=speed) {
        speed = s;
        Serial.printf("Speed=%d\n", speed);
        status+="speed=";
        status+=speed;
        status+=" &nbsp; ";
      }
    }
    if(request->hasParam("color1")) {      
      int c1 = strtol(request->getParam("color1")->value().c_str(), NULL, 16);
      if (c1!=color1) {
        color1=c1;
        Serial.printf("Color1=%06x\n", color1);
        char buf[20];
        sprintf(buf, "%06x", color1);        
        status+="color1=";
        status+=buf;
        status+=" &nbsp; ";
      }
    }
    if(request->hasParam("color2")) {
      int c2 = strtol(request->getParam("color2")->value().c_str(), NULL, 16);
      if (c2!=color2) {
        color2=c2;
        Serial.printf("Color2=%06x\n", color2);
        char buf[20];
        sprintf(buf, "%06x", color2);        
        status+="color2=";
        status+=buf;
        status+=" &nbsp; ";
      }
    }
    if(request->hasParam("nr")) {
      int p  = atoi(request->getParam("nr")->value().c_str());
      p =  (p<0) ? 0 : (p>=numPrograms ? numPrograms-1 : p);
      if (p!=activeProgram) {
        newProgram = p;
        status += " program=";
        status += newProgram;
      }
    }
    request->send(200, "text/plain", status);
  });
 
  // Start server
  server.begin();
  Serial.println("WebServer up and running!");
}

float fmap(float v, float fromMin, float fromMax, float toMin, float toMax) {
  return (v-fromMin)*(toMax-toMin)/(fromMax-fromMin)+toMin;
}

void cycleHSV(unsigned int &counter) {
  float f = (speed>100) ? fmap(speed, 100,255,2,0.2) : fmap(speed, 0,100,10,2);
  if (counter>=f*NUMPIXEL) {
    counter = 0;
  }
  for (int i=0; i<NUMPIXEL; i++) {
    pixels.setPixelColor(i, pixels.ColorHSV( ((int)((i+counter/f)*65535/NUMPIXEL))%0xffff ));
  }
  pixels.show();
}

void stars(unsigned int &counter) {
  float f = (speed>100) ? fmap(speed, 100,255, 1,10) : fmap(speed, 0,100, 0.2, 1);
  if (counter>=1000/f) {
    counter = 0;
  }
  for (int i=0; i<NUMPIXEL; i++) {
    float v = 0.1*abs(10-abs(i%20 - counter*0.02*f));
    v = 4*v-3.0;
    if (v<0) v=0.0;
/*    if (i==0) {
      Serial.printf("counter=%d, v=%f -> v=%d\n", counter, v, (int)(v*v*255));
    } */
    v = pow(v,8);
    int r = (int)(v * ((color1 >> 16) & 255));
    int g = (int)(v * ((color1 >> 8) & 255));
    int b = (int)(v * (color1 & 255));
   /* if (i==0 && r!=0) {
      Serial.printf("r=%d, g=%d, b=%d, color1=%06x, v=%f\n", r,g,b, color1, v);      
    } */
    pixels.setPixelColor(i, pixels.Color(r,g,b));
  }
  pixels.show();
}

int mix(float f) {
  int r = 0;
  for (int i=0; i<3; i++) {
    int c1 = color1 & (0xff << (8*i));
    int c2 = color2 & (0xff << (8*i));
    int c3 = (1.0-f)*c1+f*c2;
    r += c3 & (0xff<<(8*i));
  }
  //Serial.printf("mix %f of color1=%06x and color2=%06x results in %06x\n", f, color1, color2, r);
  return r;
}

int scale(float f, int color) {
  int r = 0;
  for (int i=0; i<3; i++) {
    r |= ((int)(f*(color & (0xff<<(8*i))))) & (0xff<<(8*i));
  }
  return r;
}

#define NUMFUNK 40
void funkeln(int counter) {
  static int pos[NUMFUNK];
  static int colors[NUMFUNK];
  static int count[NUMFUNK];
  static int len[NUMFUNK];
  static bool start=true;

  if (counter==0) {
    for (int i=0; i<NUMPIXEL; i++) {
      pixels.setPixelColor(i,0);
    }
  }

  if (start) {
    start = false;
    // Delay to connect to wifi will be random enough
    randomSeed(micros());
    for (int i=0; i<NUMFUNK; i++) {
      pos[i] = random(NUMPIXEL);
      colors[i] = mix(random(10001)/10000.0);
      //Serial.printf("colors[%d]=%06x\n",i,colors[i]);
      count[i] = 0;
      len[i] = (70+random(60))*(speed+20)/120.0;
    }
  } else {
    for (int i=0; i<NUMFUNK; i++) {
      count[i]++;
      float t = ((float)count[i])/len[i];
      t = t<0.5 ? 2.0*t : 2.0-2*t;
      t = pow(t,4);
      int c = scale(t,colors[i]);
      /*if (i==0) {
        Serial.printf("count=%d, len=%d, t=%f, colors[0]=%06x, c=%06x\n", count[i], len[i], t, colors[i], c);
      }*/
      pixels.setPixelColor(pos[i], c);
      if (count[i]>=len[i]) {
        pos[i] = random(NUMPIXEL);
        colors[i] = mix(random(10001)/10000.0);
        count[i] = 0;
        len[i] = (70+random(60))*(speed+20)/120.0;       
      }
    }
    pixels.show();
  }
}

void fade(unsigned int &counter) {
  if (counter>=speed*5+20) {
    counter=0;
  }
  
  float t = ((float)(counter))/(speed*5+20);
  t = cos(t*2*PI)/2+0.5;
  int c = mix(t);
  for (int i=0; i<NUMPIXEL; i++) {
    pixels.setPixelColor(i,c);
  }
  pixels.show();
}

unsigned int counter = 0; 
unsigned long nextStep = millis();
void loop() {
  ArduinoOTA.handle();
  // init
  if (newProgram!=activeProgram) {
    activeProgram = newProgram;
    nextStep = millis();
    counter = 0;
  }
  if (millis()>nextStep) {
    switch(activeProgram) {
      case 0:
        cycleHSV(counter);
        nextStep += 40;  // 25fps
        break;
      case 1:
        stars(counter);
        nextStep += 40; // 25fps
        break;
      case 2:
        funkeln(counter);
        nextStep += 40; // 25fps
        break;
      case 3:
        fade(counter);
        nextStep += 40; // 25fps
        break;
      case 4:
        break; // Do nothing, single led has been set.
    }
    counter++;
  }
}
