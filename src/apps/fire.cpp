#include "../app.h"
#include "../utils.h"
#include "../params.h"

class Fire : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Fire"; }
    virtual bool setGoodParams();
    virtual void stop();

    bool justStarted = true;
    HslColor tempToColor(float f);
    float* temp = nullptr;
};

// Color for temp in [0,1]
HslColor Fire::tempToColor(float f) {
    if (f<0.3) {
        return HslColor(0.0f, 1.0f, f/0.3*0.5);
    }
    if (f<0.7) {
        return HslColor((f-0.3)/0.4*0.1667, 1.0f, 0.5f);
    }
    return HslColor(0.166f, 1.0f, (f-0.7)/0.3f*0.5f+0.5f);
}

bool Fire::setGoodParams() {
    params.speed = 150;
    params.brightness = 60;
    params.color1 = {255,0,0};
    params.color2 = {0,255,255};
    return true;
}

void Fire::stop() {
    justStarted = true;
    if (temp!=nullptr) {
        delete[] temp;
        temp = nullptr;
    }
}


void Fire::loop() {   
    if (justStarted) {
        justStarted = false;
        if (temp) {
            delete[] temp;
        }
        temp = new float[NUMPIXEL];
        for (int i=0; i<NUMPIXEL; i++) {
            temp[i] = 0.0;
        }
    }
    for (int i=0; i<NUMPIXEL; i++) {
        if (params.posdata[i][2]<40 && random(100)==0) {
            temp[i] = 1.0;
        }
    }
    // Burn 
    for (int i=0; i<NUMPIXEL; i++) {
        for (int r=0; r<4; r++) {
            if (params.nbrs[i][r]!=0xffff) {
                temp[i]+=temp[params.nbrs[i][r]]/4;
            }
        }
        if (params.nbrs[i][5]!=0xffff) {
            float d = temp[params.nbrs[i][5]]-temp[i];
            temp[i] += d/2;
            temp[params.nbrs[i][5]] -= d/2;
        }
        temp[i] *= fmap(params.posdata[i][2], params.mins[2], params.maxs[2], 1.0, 0.3);
        if (temp[i]>1.0) temp[i]=1.0;
        if (temp[i]<0.0) temp[i]=0.0;
    }
    for (int i=0; i<NUMPIXEL; i++) {
        params.pixels->SetPixelColor(i,tempToColor(temp[i]));
    }
    params.pixels->Show();
}


// Instantiate (this will also register it into params)
Fire fire;
