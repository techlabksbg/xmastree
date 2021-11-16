#include "../app.h"
#include "../params.h"

class RainSphere : App {
    public:
    virtual void loop(unsigned int &counter);
    virtual const char* buttonName() { return "RainSphere"; }
};


void RainSphere::loop(unsigned int &counter) {
    float zero[3] {0,0,100};
    if (counter>=100) counter = 0;
    for (int i=0; i<NUMPIXEL; i++) {  
        float l=0;
        for (int k=0; k<3; k++) {
            float d = params.posdata[i][k]-zero[k];
            l+=d*d;
        }
        l = sqrt(l);
        int hue = (l/100.0+1-counter*0.01)*0xffff;
        hue %= 0xffff;
        params.pixels->setPixelColor(i, params.pixels->ColorHSV(hue));
    }
    params.pixels->show();
}

// Instantiate (this will also register it into params)
RainSphere rainsphere;
