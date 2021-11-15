#include "params.h"
#include "webserver.h"

//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXEL, PIN, NEO_GRB + NEO_KHZ800);

Params params;
WebServer* webserver;

 
void setup(){
    // Serial port for debugging purposes
    Serial.begin(115200);

    // NeoPixels
    params.pixels = new Adafruit_NeoPixel(NUMPIXEL, PIN, NEO_RGB + NEO_KHZ400);

    // Initialize SPIFFS
    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }

    // Read LED-Positions
    params.readPosData();

    // Setup WebServer:
    webserver = new WebServer();
    webserver->begin();
}


unsigned int counter = 0; 
unsigned long nextStep = millis();

void loop() {
    ArduinoOTA.handle();
    // init
    if (params.newProgram!=params.activeProgram) {
        params.activeProgram = params.newProgram;
        nextStep = millis();
        counter = 0;
    }
    if (millis()>nextStep) {
        nextStep += 40;  // 25fps;
        if (params.activeProgram>=0) {
            params.apps[params.activeProgram]->loop(counter);
        }
    }
    counter++;
}
