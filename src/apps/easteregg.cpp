#include "../app.h"
#include "../utils.h"
#include "../params.h"

class EasterEgg : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "EasterEgg"; }

};


void EasterEgg::loop() {   
    float t = secs();  // Get time in seconds
    t = t/fmap(params.speed, 0, 255, 10, 0.5);

    float centers[3][3] = {{10,4.22,140}, {10,-15,50}, {10,30,40}};
    float r = 25;
    for (int j=0; j<3; j++) {
        vec_rotxy(centers[j], centers[j], t*0.1);
    }

    for (int i=0; i<NUMPIXEL; i++) {
        
        bool inside = false;
        for (int j=0; j<3; j++) {
            if (vec_dist(params.posdata[i], centers[j])<r) {
                inside = true;
            }
        }
        if (abs(params.posdata[i][0])<=17 && abs(params.posdata[i][1])<=17 && params.posdata[i][2]>=20 && params.posdata[i][2]<=140) {
            inside = true;
        }
        bool fountain = (params.posdata[i][2] >= 140+fmod((50*t),80) && params.posdata[i][2]<=160+fmod((50*t),80) && abs(params.posdata[i][1]) <= 5 && abs(params.posdata[i][0]) <= 5);
        if (inside || fountain) {
            params.pixels->SetPixelColor(i, RgbColor(255));
        } else {
            params.pixels->SetPixelColor(i, RgbColor(0,0,20));
        }
    }
    params.pixels->Show();
}


// Instantiate (this will also register it into params)
EasterEgg easterEgg;
