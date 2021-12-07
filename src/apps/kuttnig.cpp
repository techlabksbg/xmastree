#include "../app.h"
#include "../utils.h"
#include "../params.h"

class Kuttnig : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Kuttnig"; }
    virtual bool setGoodParams();

};

bool Kuttnig::setGoodParams() {
    params.speed = 150;
    return true;
}


void Kuttnig::loop() {   
    float t = secs();  // Get time in seconds
    t = t/fmap(params.speed, 0, 255, 20, 1.0);
    for (int i=0; i<NUMPIXEL; i++) {
        float r=sqrt(params.posdata[i][0]*params.posdata[i][0] + params.posdata[i][1]*params.posdata[i][1]);
        float hue = atan2(params.posdata[i][1],params.posdata[i][0])-180.0/r*tan(r*t/2500);
        hue = fmod(hue/(2*PI),1.0);        
        float value=r/130.0;
        params.pixels->SetPixelColor(i,HslColor(hue, 1.0f, value));
    }
    params.pixels->Show();
}


// Instantiate (this will also register it into params)
Kuttnig kuttnig;
