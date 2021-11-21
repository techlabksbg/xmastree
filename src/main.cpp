#include "params.h"
#include "webserver.h"
#include "autoplayer.h"

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
    webserver->begin(true); // make ap
}


unsigned long nextStep = millis();
bool fastLoop = false;

void loop() {
    webserver->loop();    
    // init
    if (params.lastCmd+120*1000<millis()) {
        params.isAutoRunning = true;
    }
    if (params.newProgram!=params.activeProgram) {
        params.activeProgram = params.newProgram;
        fastLoop = (params.activeProgram>=0 && params.apps[params.activeProgram]->loopFast());
        nextStep = millis();
        params.pixels->clear();
        params.pixels->show();
        Serial.printf("Now running %s\n", params.apps[params.activeProgram]->buttonName());
    }
    if (millis()>nextStep || fastLoop) {
        nextStep = millis()+40;  // 25fps;
        if (params.activeProgram>=0) {
            params.apps[params.activeProgram]->loop();
        }
    }
    if (params.isAutoRunning) {
        autoplayer();
    }
}
