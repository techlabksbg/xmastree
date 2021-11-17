#include "../app.h"
#include "../utils.h"
#include "../params.h"

class Stars : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Stars"; }
};


void Stars::loop() {
    float dist = 25;
    float t = fmod(secs()/fmap(params.speed, 0, 255, 20, 1.0),1.0);
    for (int i=0; i<NUMPIXEL; i++) {
        float v = abs(fmod((i+t*dist),dist)-dist/2)/(dist/2); // Triangle function [0,dist] -> [0,1]
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
