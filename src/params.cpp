#include "params.h"

#include <WiFiUdp.h>



void Params::begin() {
    // NeoPixels
#ifdef WIFIDEBUG        
    pixels = new MyNeoPixel(NUMPIXEL, PIN);
#else
    pixels = new PIXELCONFIG(NUMPIXEL, PIN);
#endif
    pixels->Begin();
    readPosData();
}

void Params::readPosData() {
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

int Params::getAppId(const char* name) {
    for (int i=0; i<apps.size(); i++) {
        if (strcmp(name, apps[i]->buttonName())==0) {
            return i;
        }
    }
    return -1;
}

#ifdef WIFIDEBUG


MyNeoPixel::MyNeoPixel(uint16_t n, int16_t pin) :
        PIXELCONFIG(n,pin) 
{
    buffer = new byte[3*NUMPIXEL+6];
    buffer[0] = 'M';
    buffer[1] = 'a';
    buffer[2] = 'G';
    buffer[3] = 'i';
    buffer[4] = 'C';
    buffer[5] = '0';
    colorData = buffer+6;
}



void MyNeoPixel::Show() {
    PIXELCONFIG::Show();

    udp.begin(10000);

    // get ColorData
    for (int i=0; i<NUMPIXEL; i++) {
        RgbColor c = GetPixelColor(i);
        colorData[i*3] = c.R;
        colorData[i*3+1] = c.G;
        colorData[i*3+2] = c.B;
    }
    // send ColorData
    //Serial.println("Sending MaGiC0 data");
    udp.beginPacket("192.168.42.2", 10000);
    udp.write(buffer, 1506);
    udp.endPacket();
}


#endif