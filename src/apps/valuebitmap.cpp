#include "../app.h"
#include "../utils.h"
#include "../params.h"
#include <SPIFFS.h>

class ValueBitmap : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Tech-Lab"; }
    virtual bool loopFast() { return true; }
    virtual bool setGoodParams();
    virtual void stop();

    private:
    File bitmap;
    const char*files[2] = {"/values.bin", "/ksbg.bin"};
    int active = 0;
    char* values = nullptr;
    unsigned int nextFrame = 0;

};

bool ValueBitmap::setGoodParams() {
    params.speed = 255;
    params.color1 = 0xffaa00;
    return false;
}

void ValueBitmap::stop() {
    Serial.println("Stopping ValueBitmap");
    if (bitmap) {
        Serial.println("  bitmap close");
        bitmap.close();
    }
    active = (active+1)%2;
    nextFrame = 0;
    if (values!=nullptr) {
        Serial.println("delete[] values");
        delete[] values;
        values = nullptr;
    }
}

void ValueBitmap::loop() {    
    if (millis()>nextFrame) {
        nextFrame = millis()+fmap(params.speed, 0, 255, 200, 4);
        if (!bitmap) { // Open file if necessary
            bitmap = SPIFFS.open(files[active]);
        }
        if (values==nullptr) { // Allocate buffer, if necessary
            values = new char[NUMPIXEL];
        }
        bitmap.readBytes(values, NUMPIXEL);
        for (int i=0; i<NUMPIXEL; i++) {
            params.pixels->setPixelColor(i, scale(values[i]/255.0,params.color1));
        }
        params.pixels->show();
        if (!bitmap.available()) {
            stop();
        }
    }
}


// Instantiate (this will also register it into params)
ValueBitmap valuebitmap;
