#pragma once
#include <Arduino.h>
#include <vector>
#include <NeoPixelBus.h>
#include "SPIFFS.h"
#include "app.h"
#include "SD.h"



#define PIN 13
#define NUMPIXEL 500
// See https://github.com/Makuna/NeoPixelBus/wiki/ESP32-NeoMethods
#define PIXELCONFIG NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod>

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
    int drawx = 0;
    int drawy = 0;
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
    //Adafruit_NeoPixel* pixels;
    PIXELCONFIG* pixels;// strip(PixelCount, PixelPin);
#endif
    std::vector<float> mins = std::vector<float> (3, 500);
    std::vector<float> maxs = std::vector<float> (3, -500);
    
    void readPosData();
    void begin();
    int getAppId(const char* name);

};


extern Params params;

