#include "../app.h"
#include "../utils.h"
#include "../params.h"

class Fade : App {
    public:
    virtual void loop(unsigned int &counter);
    const char* buttonName = "Fade";
};


void Fade::loop(unsigned int &counter) {
    if (counter>=params.speed*5+20) {
        counter=0;
    }    
    float t = ((float)(counter))/(params.speed*5+20);
    t = cos(t*2*PI)/2+0.5;
    int c = mix(t);
    for (int i=0; i<NUMPIXEL; i++) {
        params.pixels->setPixelColor(i,c);
    }
    params.pixels->show();
}


// Instantiate (this will also register it into params)
Fade fade;
