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
    MyNeoPixel(uint16_t n, int16_t pin = (int16_t)6, neoPixelType type = (neoPixelType)82U) :
        Adafruit_NeoPixel(n,pin,type) {
            buffer = new byte[3*NUMPIXEL];
    }

    WiFiClient client;
    
    void begin() {
        client.connect("192.168.42.2", 10000);
        if (client) {
            //Serial.println("Connection to 192.168.42.2:10000 established!");
        } else {
            //Serial.println("Connection to 192.168.42.2:10000 failed!");
        }
    }

    void show() {
        Adafruit_NeoPixel::show();
        if (!client) {
            begin();
            if (!client) {
                //Serial.println("abort show");
                return;
            }
        } else if (!client.connected() || !client.availableForWrite()) {
            //Serial.println("Lost connection!");
            client.stop();
            return;
        }

        // get ColorData
        for (int i=0; i<NUMPIXEL; i++) {
            int c = getPixelColor(i);
            buffer[i*3] = c >> 16;
            buffer[i*3+1] = (c>>8) & 0xff;
            buffer[i*3+2] = c & 0xff;
        }
        // send ColorData
        //Serial.println("Sending color data");
        client.write(buffer, 1500);
        client.stop();
    }
    
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

