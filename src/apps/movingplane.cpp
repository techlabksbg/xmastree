#include "../app.h"
#include "../utils.h"
#include "../params.h"


class MovingPlane : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Up'n down"; }
    virtual bool setGoodParams();
};

bool MovingPlane::setGoodParams() {
    static int p = 0;
    params.speed = 200;
    params.brightness = 50;
    switch (p) {
    case 0:
        params.color1 = 0x1111ff;
        params.color2 = 0xff2266;
        break;
    case 1:
        params.color1 = 0xff0000;
        params.color2 = 0x00ff00;
        params.speed = 100;
        break;

    }
    p = (p+1)%2;
    return false;
}


void MovingPlane::loop() {
    float t = secs()/fmap(params.speed, 0, 255, 20, 1.0);
    int dir = ((int)t) % 3;
    float normal[3] {dir==0 ? 1.0f : 0.0f, dir==1 ? 1.0f : 0.0f, dir==2 ? 1.0f : 0.0f};
    float d = cos(t*2*PI)*(params.maxs[dir]-params.mins[dir])/2+(params.maxs[dir]+params.mins[dir])/2;

    for (int i=0; i<NUMPIXEL; i++) {
        float sp = normal[0]*params.posdata[i][0]+normal[1]*params.posdata[i][1]+normal[2]*params.posdata[i][2]-d;
        params.pixels->SetPixelColor(i, sp>0 ? params.color1 : params.color2);
    }
    params.pixels->Show();
}


// Instantiate (this will also register it into params)
MovingPlane movingplane;
