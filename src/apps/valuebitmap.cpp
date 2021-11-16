#include "../app.h"
#include "../utils.h"
#include "../params.h"
#include <SPIFFS.h>

class ValueBitmap : App {
    public:
    virtual void loop(unsigned int &counter);
    virtual const char* buttonName() { return "Tech-Lab"; }
};


void ValueBitmap::loop(unsigned int &counter) {
   static File bitmap;
    static char values[NUMPIXEL];
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


// Instantiate (this will also register it into params)
ValueBitmap valuebitmap;
