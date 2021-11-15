#pragma once
#include <Arduino.h>
#include <vector>
#include <Adafruit_NeoPixel.h>
#include "SPIFFS.h"
#include "app.h"

#define PIN 13
#define NUMPIXEL 200



struct Params {
    int activeProgram = 0;
    int newProgram = 2;
    int numPrograms = 9;
    int brightness = 255;
    int speed = 35;
    uint32_t color1 = 0xff0000;
    uint32_t color2 = 0x0000ff;
    float posdata[NUMPIXEL][3];
    std::vector<App*> apps;
    Adafruit_NeoPixel* pixels;

    void readPosData() {
        File f = SPIFFS.open("/posdata.txt", FILE_READ);
        if (f) {
            for (int i=0; i<NUMPIXEL; i++) {
                for (int k=0; k<3; k++) {
                    posdata[i][k] = f.parseFloat();
                }
            }
            f.close();
        }
    }
};


extern Params params;



