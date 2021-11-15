#include "../app.h"
#include "../utils.h"
#include "../params.h"


class MovingPlane : App {
    public:
    virtual void loop(unsigned int &counter);
    const char* buttonName = "Up'n down";
};


void MovingPlane::loop(unsigned int &counter) {
   float normal[3] {0,0,1};
    float d = sin(counter*0.04)*75+110.0;
    for (int i=0; i<NUMPIXEL; i++) {
        float sp = normal[0]*params.posdata[i][0]+normal[1]*params.posdata[i][1]+normal[2]*params.posdata[i][2]-d;
        params.pixels->setPixelColor(i, sp>0 ? params.color1 : params.color2);
    }
    params.pixels->show();
}


// Instantiate (this will also register it into params)
MovingPlane movingplane;
