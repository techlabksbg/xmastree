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

//#define WIFIDEBUG

#ifdef WIFIDEBUG
#include "WiFi.h"
class MyNeoPixel : public Adafruit_NeoPixel {
    public:
    byte* buffer;
    byte* colorData;
    MyNeoPixel(uint16_t n, int16_t pin = (int16_t)6, neoPixelType type = (neoPixelType)82U);

    WiFiUDP udp;

    void show();
};

#endif

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
#ifdef WIFIDEBUG
    MyNeoPixel* pixels;
#else
    Adafruit_NeoPixel* pixels;
#endif
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

    void begin() {
        // NeoPixels
#ifdef WIFIDEBUG        
        pixels = new MyNeoPixel(NUMPIXEL, PIN, NEO_RGB + NEO_KHZ800);
#else
        pixels = new Adafruit_NeoPixel(NUMPIXEL, PIN, NEO_RGB + NEO_KHZ800);
#endif
        readPosData();
    }

};


extern Params params;

