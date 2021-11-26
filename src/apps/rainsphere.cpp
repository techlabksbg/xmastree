#include "../app.h"
#include "../params.h"
#include "../utils.h"

class RainSphere : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "RainSphere"; }
    float zero[3] {0,0,100};
    int dir = 1;
    virtual bool setGoodParams();

};

bool RainSphere::setGoodParams() {
    params.speed = 170;
    params.brightness = 127;
    return true;
}

void RainSphere::loop() {
    zero[2] += (float)dir * params.speed / 1000;
    if ((dir == 1 && zero[2] > params.maxs[2] )||(dir == -1 && zero[2] < params.mins[2])){
        dir *= -1;
        
    }
    float t = fmod(secs()/fmap(params.speed, 0, 255, 20, 1.0),1.0);

    for (int i=0; i<NUMPIXEL; i++) {  
        float l=0;
        for (int k=0; k<3; k++) {
            float d = params.posdata[i][k]-zero[k];
            l+=d*d;
        }
        l = sqrt(l);
        int hue = fmod(l/100.0+(1-t),1.0)*0xffff;
        hue %= 0xffff;
        params.pixels->setPixelColor(i, params.pixels->ColorHSV(hue));
    }
    params.pixels->show();
}

// Instantiate (this will also register it into params)
RainSphere rainsphere;
