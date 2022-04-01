#pragma once
#include <Arduino.h>
#include <vector>
#include <NeoPixelBus.h>
#include "SPIFFS.h"
#include "app.h"
#include "SD.h"
#include "mypixel.h"



// Pins for SD-Card
#define SD_CS          5
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18

#define NUMPIXEL 500

struct Params {
    int activeProgram = 0;
    int newProgram = 0;
    int brightness = 255;
    int speed = 200;
    int singlePixel = -1;
    int drawx = 0;
    int drawy = 0;
    bool isAutoRunning = false;
    unsigned long lastCmd = 0;
    RgbColor color1 = RgbColor(255,0,0);
    RgbColor color2 = RgbColor(0,0,255);
    String text = "TECHLAB";
    float posdata[NUMPIXEL][2];
    std::vector<App*> apps;
    MyPixel* pixels;// strip(PixelCount, PixelPin);
    std::vector<float> mins = std::vector<float> (2, 500);
    std::vector<float> maxs = std::vector<float> (2, -500);
    
    void readPosData();
    void begin();
    int getAppId(const char* name);

};


extern Params params;

