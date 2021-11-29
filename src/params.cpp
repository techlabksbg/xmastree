#include "params.h"

#include <WiFiUdp.h>

#ifdef WIFIDEBUG


MyNeoPixel::MyNeoPixel(uint16_t n, int16_t pin, neoPixelType type) :
        Adafruit_NeoPixel(n,pin,type) 
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



void MyNeoPixel::show() {
    Adafruit_NeoPixel::show();

    udp.begin(10000);

    // get ColorData
    for (int i=0; i<NUMPIXEL; i++) {
        int c = getPixelColor(i);
        colorData[i*3] = c >> 16;
        colorData[i*3+1] = (c>>8) & 0xff;
        colorData[i*3+2] = c & 0xff;
    }
    // send ColorData
    //Serial.println("Sending MaGiC0 data");
    udp.beginPacket("192.168.42.2", 10000);
    udp.write(buffer, 1506);
    udp.endPacket();
}


#endif