#include "../app.h"
#include "../utils.h"
#include "../params.h"



class PlaneSnow : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Plane Snow"; }
    virtual bool setGoodParams();

    private:
    float n[3];
    float dm[2];
    bool started = false;
    float t = 0.0;

    void start();

};

void PlaneSnow::start() {
    for (int i=0; i<3; i++) {
        n[i] = rand()/(float)RAND_MAX;
        if (i<2) {
            n[i] = 2*n[i]-1.0;
        }
    }
    vec_mul(n, 1.0/vec_norm(n));
    dm[0] = vec_dot(n, params.posdata[0]);
    dm[1] = dm[0];
    for (int i=1; i<NUMPIXEL; i++) {
        float d = vec_dot(n, params.posdata[i]);
        if (d<dm[0]) dm[0] = d;
        if (d>dm[1]) dm[1] = d;
    }
}

bool PlaneSnow::setGoodParams() {
    params.speed = 180;
    params.brightness = 60;
    params.color1 = {255,255,255};
    params.color2 = {0,0,255};
    return true;
}


void PlaneSnow::loop() {   
    float sec = secs()/fmap(params.speed, 0, 255, 10, 1.0)-t;
    if (sec<0 || sec>1) {
        start();
        sec = secs()/fmap(params.speed, 0, 255, 10, 1.0);
        t = sec;
        sec = 0.0;
    }
    float d = sec*(dm[0]-dm[1])+dm[1];
    RgbColor back = params.color2;
    back.Darken(230);
    for (int i=0; i<NUMPIXEL; i++) {
        float dd = vec_dot(n, params.posdata[i])-d;
        if (dd<20 && dd>0) {            
            params.pixels->SetPixelColor(i,RgbColor::LinearBlend(params.color1, back, dd/20));
        } else {
            params.pixels->SetPixelColor(i,back);
        }
    }
    params.pixels->Show();
}


// Instantiate (this will also register it into params)
//PlaneSnow planeSnow;
