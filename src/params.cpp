#include "params.h"

#ifdef WIFIDEBUG


MyNeoPixel::MyNeoPixel(uint16_t n, int16_t pin, neoPixelType type) :
        Adafruit_NeoPixel(n,pin,type) 
{
    buffer = new byte[3*NUMPIXEL];
}


void MyNeoPixel::begin() {
    client.connect("192.168.42.2", 10000);
    if (client) {
        //Serial.println("Connection to 192.168.42.2:10000 established!");
    } else {
        //Serial.println("Connection to 192.168.42.2:10000 failed!");
    }
}

void MyNeoPixel::show() {
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


#endif