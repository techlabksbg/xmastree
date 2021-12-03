#include "../app.h"
#include "../utils.h"
#include "../params.h"

class HSVCycle : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "HSV Cycle"; }
    virtual bool setGoodParams();
};

bool HSVCycle::setGoodParams() {
    params.brightness = 50;
    params.speed = 200;
    return true;
}

void HSVCycle::loop() {
    float t = secs()/fmap(params.speed, 0, 255, 20, 1.0);
    for (int i=0; i<NUMPIXEL; i++) {
        float h = fmod(((float)i)/NUMPIXEL+t,1.0);
        params.pixels->SetPixelColor(i, HslColor(h,1.0f,1.0f));
    }
    params.pixels->Show();
}

// Instantiate (this will also register it into params)
HSVCycle hsvcycle;
