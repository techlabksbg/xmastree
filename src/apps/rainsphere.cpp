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
    static int p = 0;
    if (p==0) {
        params.speed = 170;
    } else {
        params.speed = 250;
    }
    params.brightness = 70;
    p = (p+1)%2;
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
        float hue = fmod(l/100.0+(1-t),1.0);
        params.pixels->SetPixelColor(i, HslColor(hue,1.0f,0.5f));
    }
    params.pixels->Show();
}

// Instantiate (this will also register it into params)
RainSphere rainsphere;
