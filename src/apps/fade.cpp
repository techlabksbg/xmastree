#include "../app.h"
#include "../utils.h"
#include "../params.h"

class Fade : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Fade"; }
    virtual bool setGoodParams();

};

bool Fade::setGoodParams() {
    params.speed = 150;
    params.brightness = 60;
    params.color1 = {255,0,0};
    params.color2 = {0,255,255};
    return true;
}


void Fade::loop() {   
    float t = secs();  // Get time in seconds
    t = t/fmap(params.speed, 0, 255, 20, 1.0);

    float w = cos(t*2*PI)/2+0.5;
    RgbColor c = mix(w);
    for (int i=0; i<NUMPIXEL; i++) {
        params.pixels->SetPixelColor(i,c);
    }
    params.pixels->Show();
}


// Instantiate (this will also register it into params)
Fade fade;
