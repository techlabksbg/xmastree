#include "params.h"
#include "utils.h"




void Params::begin() {
    // NeoPixels
    pixels = new MyPixel();
    pixels->Begin();
    readPosData();
}

void Params::readPosData() {
    File f = SPIFFS.open("/posdata.txt", FILE_READ);
    if (f) {
        for (int i=0; i<NUMPIXEL; i++) {
            for (int k=0; k<2; k++) {
                posdata[i][k] = f.parseFloat();
                if (posdata[i][k] > maxs[k]){
                    maxs[k] = posdata[i][k];
                }
                if (posdata[i][k] < mins[k]){
                    mins[k] = posdata[i][k];
                }
            }
        }
        f.close();
    } else {
        Serial.println("Could not read position data! Store it on SPIFFS at posdata.txt");
    }
}

int Params::getAppId(const char* name) {
    for (int i=0; i<apps.size(); i++) {
        if (strcmp(name, apps[i]->buttonName())==0) {
            return i;
        }
    }
    return -1;
}

