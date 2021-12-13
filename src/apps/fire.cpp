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
    float* diff = nullptr;
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
    return false;
}

void Fire::stop() {
    justStarted = true;
    if (temp!=nullptr) {
        delete[] temp;
        temp = nullptr;
    }
    if (diff!=nullptr) {
        delete[] diff;
        diff = nullptr;
    }
}


void Fire::loop() {   
    if (justStarted) {
        justStarted = false;
        if (temp) {
            delete[] temp;
        }
        if (diff) {
            delete[] diff;
        }
        temp = new float[NUMPIXEL];
        diff = new float[NUMPIXEL];
        for (int i=0; i<NUMPIXEL; i++) {
            temp[i] = 0.0f;
            diff[i] = 0.0f;
        }
    }
    for (int i=0; i<NUMPIXEL; i++) {
        diff[i] = 0.0f;
        if (params.posdata[i][2]<40 && random(100)==0) {
            temp[i] = 1.0;
        }
    }
    // Burn 
    for (int i=0; i<NUMPIXEL; i++) {

        // spread around
        for (int r=0; r<4; r++) {
            if (params.nbrs[i][r]!=0xffff) {
                int j = params.nbrs[i][r];
                float d = (temp[j] - temp[i])/20;
                diff[i]+=d/10;
                diff[j]-=d/10;
             } else {
                 diff[i]-=temp[i]/5;
             }
        }
        if (params.nbrs[i][5]!=0xffff) {
            int j = params.nbrs[i][5];
            float d = (temp[j]-temp[i])/5;
            diff[i] += d;
            diff[j] -= d;
        } else {
            diff[i] -= temp[i]/5;
        }
        //diff[i] += temp[i]*fmap(params.posdata[i][2], params.mins[2], params.maxs[2], 0.2, -0.1);
        temp[i] += diff[i];
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
