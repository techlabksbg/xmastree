#include "../app.h"
#include "../utils.h"
#include "../params.h"

class Stars : App {
    public:
    virtual void loop(unsigned int &counter);
    const char* buttonName = "Stars";
};


void Stars::loop(unsigned int &counter) {
    float f = (params.speed>100) ? fmap(params.speed, 100,255, 1,10) : fmap(params.speed, 0,100, 0.2, 1);
    if (counter>=1000/f) {
        counter = 0;
    }
    for (int i=0; i<NUMPIXEL; i++) {
        float v = 0.1*abs(10-abs(i%20 - counter*0.02*f));
        v = 4*v-3.0;
        if (v<0) v=0.0;
    /*    if (i==0) {
        Serial.printf("counter=%d, v=%f -> v=%d\n", counter, v, (int)(v*v*255));
        } */
        v = pow(v,8);
        int r = (int)(v * ((params.color1 >> 16) & 255));
        int g = (int)(v * ((params.color1 >> 8) & 255));
        int b = (int)(v * (params.color1 & 255));
    /* if (i==0 && r!=0) {
        Serial.printf("r=%d, g=%d, b=%d, color1=%06x, v=%f\n", r,g,b, color1, v);      
        } */
        params.pixels->setPixelColor(i, params.pixels->Color(r,g,b));
    }
    params.pixels->show();
}

// Instantiate (this will also register it into params)
Stars stars;
