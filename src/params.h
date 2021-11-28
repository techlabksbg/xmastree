#pragma once
#include <Arduino.h>
#include <vector>
#include <Adafruit_NeoPixel.h>
#include "SPIFFS.h"
#include "app.h"
#include "SD.h"

#define PIN 13
#define NUMPIXEL 500

// Pins for SD-Card
#define SD_CS          5
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18



struct Params {
    int activeProgram = 4;
    int newProgram = 2;
    int brightness = 255;
    int speed = 200;
    int singlePixel = -1;
    bool isAutoRunning = false;
    unsigned long lastCmd = 0;
    uint32_t color1 = 0xff0000;
    uint32_t color2 = 0x0000ff;
    String text = "TECHLAB";
    float posdata[NUMPIXEL][3];
    std::vector<App*> apps;
    Adafruit_NeoPixel* pixels;
    std::vector<float> mins = std::vector<float> (3, 500);
    std::vector<float> maxs = std::vector<float> (3, -500);

    void readPosData() {
        File f = SPIFFS.open("/posdata.txt", FILE_READ);
        if (f) {
            for (int i=0; i<NUMPIXEL; i++) {
                for (int k=0; k<3; k++) {
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
        }
    }

    
};


extern Params params;



