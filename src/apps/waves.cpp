#include "../app.h"
#include "../utils.h"
#include "../params.h"


class Wave : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Wave"; }
    virtual bool setGoodParams();

};

bool Wave::setGoodParams() {
    params.brightness = 255;    
    return true;
}

void Wave::loop() {
    float t = fmod(secs()/fmap(params.speed, 0, 255, 40, 2.0),1.0);
    for (int i=0; i<NUMPIXEL; i++) {
        float x = params.posdata[i][0];
        float y = params.posdata[i][1];
        float z = params.posdata[i][2];
        float d = sqrt(x*x+y*y);
        float f = sin(d/10)*20/(1+d*d/500)+60+sin(t/5);
        int h = fmod(t/4+d/1000.0,1.0)*0xffff;
        int v = z<f ? 255 : 0;
        params.pixels->setPixelColor(i, params.pixels->ColorHSV(h,255,v));
    }
    params.pixels->show();
}


// Instantiate (this will also register it into params)
Wave wave;
