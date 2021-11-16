#include "../app.h"
#include "../utils.h"
#include "../params.h"

class HSVCycle : App {
    public:
    virtual void loop(unsigned int &counter);
    virtual const char* buttonName() { return "HSV Cycle"; }
};


void HSVCycle::loop(unsigned int &counter) {
    float f = (params.speed>100) ? fmap(params.speed, 100,255,2,0.2) : fmap(params.speed, 0,100,10,2);
    if (counter>=f*NUMPIXEL) {
        counter = 0;
    }
    for (int i=0; i<NUMPIXEL; i++) {
        params.pixels->setPixelColor(i, params.pixels->ColorHSV( ((int)((i+counter/f)*65535/NUMPIXEL))%0xffff ));
    }
    params.pixels->show();
}

// Instantiate (this will also register it into params)
HSVCycle hsvcycle;
