#include "../app.h"
#include "../utils.h"
#include "../params.h"

class HSVCycle : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "HSV Cycle"; }
};


void HSVCycle::loop() {
    float t = secs()/fmap(params.speed, 0, 255, 20, 1.0);
    for (int i=0; i<NUMPIXEL; i++) {
        float h = fmod(((float)i)/NUMPIXEL+t,1.0);
        params.pixels->setPixelColor(i, params.pixels->ColorHSV((int)(h*65535)));
    }
    params.pixels->show();
}

// Instantiate (this will also register it into params)
HSVCycle hsvcycle;
