#include "../app.h"
#include "../utils.h"
#include "../params.h"
#include <SPIFFS.h>

class ValueBitmap : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Tech-Lab"; }
    virtual bool loopFast() { return true; }
};


void ValueBitmap::loop() {
    static File bitmap;
    static char values[NUMPIXEL];
    static unsigned int nextFrame = 0;
    if (millis()>nextFrame) {
        nextFrame = millis()+fmap(params.speed, 0, 255, 200, 4);
        if (!bitmap) {
            bitmap = SPIFFS.open("/values.bin");
            Serial.println("values.bin opened");
        }
        bitmap.readBytes(values, NUMPIXEL);
        for (int i=0; i<NUMPIXEL; i++) {
            params.pixels->setPixelColor(i, scale(values[i]/255.0,params.color1));
        }
        params.pixels->show();
        if (! bitmap.available()) {
            bitmap.close();
        }
    }
}


// Instantiate (this will also register it into params)
ValueBitmap valuebitmap;
