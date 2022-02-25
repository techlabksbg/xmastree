#include "../app.h"
#include "../utils.h"
#include "../params.h"

class Stars : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Stars"; }
    virtual bool setGoodParams();
};

bool Stars::setGoodParams() {
    params.speed = 200;
    params.color1 = {255,0,0};
    params.color2 = {0,0,255};
    return true;
}
void Stars::loop() {
    float dist = 37;
    float t = secs()/fmap(params.speed, 0, 255, 20, 1.0);
    RgbColor color = mix((sin(t*2*PI)+1)/2);
    for (int i=0; i<NUMPIXEL; i++) {
        float v = abs(fmod((i+t*dist),dist)-dist/2)/(dist/2); // Triangle function [0,dist] -> [0,1]
        v = pow(v,8);
        RgbColor c = scale(v, color);
        params.pixels->SetPixelColor(i, c);
    }
    params.pixels->Show();
}

// Instantiate (this will also register it into params)
//Stars stars;
