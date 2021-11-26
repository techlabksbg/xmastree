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

};

bool ValueBitmap::setGoodParams() {
    params.speed = 255;
    params.color1 = 0xffaa00;
    return true;
}

void ValueBitmap::loop() {
    static File bitmap;
    static const char*files[] ={"/values.bin", "/ksbg.bin"};
    static int active = 0;
    static char values[NUMPIXEL];
    static unsigned int nextFrame = 0;
    if (millis()>nextFrame) {
        nextFrame = millis()+fmap(params.speed, 0, 255, 200, 4);
        if (!bitmap) {
            bitmap = SPIFFS.open(files[active]);
        }
        bitmap.readBytes(values, NUMPIXEL);
        for (int i=0; i<NUMPIXEL; i++) {
            params.pixels->setPixelColor(i, scale(values[i]/255.0,params.color1));
        }
        params.pixels->show();
        if (! bitmap.available()) {
            bitmap.close();
            active = (active+1)%2;
        }
    }
}


// Instantiate (this will also register it into params)
ValueBitmap valuebitmap;
