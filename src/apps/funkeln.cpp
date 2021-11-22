#include "../app.h"
#include "../utils.h"
#include "../params.h"

#define NUMFUNK 40

class Funkeln : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Funkeln"; }
    virtual bool setGoodParams();
};

bool Funkeln::setGoodParams() {
    static int p = 0;
    switch (p) {
    case 0:
        params.color1 = 0x1111ff;
        params.color2 = 0xff2266;
        params.speed = 100;
        break;
    case 1:
        params.color1 = 0xff0000;
        params.color2 = 0x00ff00;
        params.speed = 200;
        break;

    }
    p = (p+1)%2;
    return true;
}

void Funkeln::loop() {
    static int pos[NUMFUNK];
    static int colors[NUMFUNK];
    static int count[NUMFUNK];
    static int len[NUMFUNK];

    if (secs()==0.0) {
        for (int i=0; i<NUMPIXEL; i++) {
            params.pixels->setPixelColor(i,0);
        }
        // Delay to connect to wifi will be random enough
        randomSeed(micros());
        for (int i=0; i<NUMFUNK; i++) {
            pos[i] = random(NUMPIXEL);
            colors[i] = mix(random(10001)/10000.0);
            //Serial.printf("colors[%d]=%06x\n",i,colors[i]);
            count[i] = 0;
            len[i] = (70+random(60))*((255-params.speed)+20)/120.0;
        }
    } else {
        for (int i=0; i<NUMFUNK; i++) {
            count[i]++;
            float t = ((float)count[i])/len[i];
            t = t<0.5 ? 2.0*t : 2.0-2*t;
            t = pow(t,4);
            int c = scale(t,colors[i]);
            /*if (i==0) {
                Serial.printf("count=%d, len=%d, t=%f, colors[0]=%06x, c=%06x\n", count[i], len[i], t, colors[i], c);
            }*/
            params.pixels->setPixelColor(pos[i], c);
            if (count[i]>=len[i]) {
                pos[i] = random(NUMPIXEL);
                colors[i] = mix(random(10001)/10000.0);
                count[i] = 0;
                len[i] = (70+random(60))*((255-params.speed)+20)/120.0;       
            }
        }
    }
    params.pixels->show();
}


// Instantiate (this will also register it into params)
Funkeln funkeln;
