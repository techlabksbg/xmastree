#include "../app.h"
#include "../utils.h"
#include "../params.h"

class Fade : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Fade"; }
};


void Fade::loop() {   
    float t = secs();  // Get time in seconds
    t = t/fmap(params.speed, 0, 255, 20, 1.0);

    float w = cos(t*2*PI)/2+0.5;
    int c = mix(w);
    for (int i=0; i<NUMPIXEL; i++) {
        params.pixels->setPixelColor(i,c);
    }
    params.pixels->show();
}


// Instantiate (this will also register it into params)
Fade fade;
