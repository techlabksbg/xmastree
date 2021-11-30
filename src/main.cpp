#include "params.h"
#include "webserver.h"
#include "autoplayer.h"

//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXEL, PIN, NEO_GRB + NEO_KHZ800);

Params params;
WebServer* webserver;

 
void setup(){
    // Serial port for debugging purposes
    Serial.begin(115200);


    // Initialize SPIFFS
    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }

    // Initialize SD Card
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);  // See param.h for pin definition
    SD.begin(SD_CS);
    Serial.println("SD card init over");

    // Read LED-Positions, Initialize NeoPixels
    params.begin();

    // Setup WebServer:
    webserver = new WebServer();
    webserver->begin(true); // true = make ap, false = connect to tech-lab
}


unsigned long nextStep = millis();
bool fastLoop = false;

void loop() {
    webserver->loop();    
    // init
    if (params.lastCmd+120*1000<millis() || params.lastCmd == 0) {
        params.isAutoRunning = true;
    }
    if (params.newProgram!=params.activeProgram) {
        // Stop current app
        if (params.activeProgram>=0) {
            Serial.printf("Stoping %s\n", params.apps[params.activeProgram]->buttonName());
            params.apps[params.activeProgram]->stop();
        }
        params.activeProgram = params.newProgram;
        fastLoop = (params.activeProgram>=0 && params.apps[params.activeProgram]->loopFast());
        nextStep = millis();
        if (params.activeProgram>=0) {
            params.pixels->clear();
            params.pixels->show();
        } else if (params.singlePixel>=0) {
            params.pixels->clear();
            params.pixels->setPixelColor(params.singlePixel, 255,255,255);
        }
        if (params.activeProgram>=0) {
            Serial.printf("Now running %s\n", params.apps[params.activeProgram]->buttonName());
        }
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
