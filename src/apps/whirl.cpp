#include "../app.h"
#include "../utils.h"
#include "../params.h"


class Whirl : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Whirl"; }
};


void Whirl::loop() {
    float t = fmod(secs()/fmap(params.speed, 0, 255, 40, 2.0),1.0);
    float w = t*60;
    float h = 20+t*180;
    float r = 35-t*30;
    float p[3] {(float)(r*cos(w)), (float)(r*sin(w)), h};
    for (int i=0; i<NUMPIXEL; i++) {
        float l = 0.0;
        for (int k=0; k<3; k++) {  
            float d = params.posdata[i][k]-p[k];
            l+=d*d;
        }
        float ball = 10*10;
        if (l<ball) {
            l = 1.0;  
        } else {
            l = pow(ball/l,3);
        }
        params.pixels->setPixelColor(i, scale(l, mix(t)));
    }
    params.pixels->show();
}


// Instantiate (this will also register it into params)
Whirl whirl;
