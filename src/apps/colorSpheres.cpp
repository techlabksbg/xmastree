#include "../app.h"
#include "../params.h"
#include "../utils.h"

class ColorSphere : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "farbige Kugeln"; }
    float zero[3] {0,0,100};
    int dir = 1;
    virtual bool setGoodParams();

    private:
    void start();
    int centerLed = 0;
    float maxDist = 0.0f;
    HslColor f1 = HslColor(0.3, 1, 0.5);
    HslColor f2 = HslColor(0,1,0.5);
    HslColor f3 = HslColor(0, 0, 0.5);
    int colorScheme = 0;
    float lastStart = -100.0f;
};

void ColorSphere::start() {
    centerLed = random(NUMPIXEL);
    maxDist = 0.0f;
    for (int i=0; i<NUMPIXEL; i++) {
        float d = vec_dist(params.posdata[i], params.posdata[centerLed]);
        if (d>maxDist) maxDist = d;
    }
    if (colorScheme==0) {
        f1 = HslColor(195.0/360, 1, 0.5); //  #blaucyan
        f2 = HslColor(1/4, 1, 0.5);
    } else if (colorScheme==1) {
        f1 = HslColor(345.0/360, 1, 0.5); //    #leichtes Blaurot
        f2 = HslColor(195.0/360, 1, 0.5);
    } else if (colorScheme==2) {
        f1 = HslColor(1.0/4, 1, 0.5);
        f2 = HslColor(145.0/360, 1, 0.5);
    }    

    colorScheme=(colorScheme+1)%3;
}

bool ColorSphere::setGoodParams() {
    params.speed = 200;
    return false;
}

void ColorSphere::loop() {
    float t = secs()/fmap(params.speed, 0, 255, 20, 1.0);
    if (t-lastStart>1.0 || t-lastStart<0.0) {
        lastStart = t;
        start();
    }
    float r = (t-lastStart)*(maxDist+10);
    for (int i=0; i<NUMPIXEL; i++) {  
        float d = vec_dist(params.posdata[centerLed], params.posdata[i]);
        if (d<r+10 && d>r-10) {
            params.pixels->SetPixelColor(i, f3);
        } else if (d<r+10) {
            params.pixels->SetPixelColor(i, f1);
        } else {
            params.pixels->SetPixelColor(i, f2);
        }
    }
    params.pixels->Show();
}

// Instantiate (this will also register it into params)
//ColorSphere colorSphere;
