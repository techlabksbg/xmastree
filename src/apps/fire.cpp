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
        return HslColor(0.0f, 1.0f, f*f/0.3/0.3*0.5);
        //return HslColor(0.0f, 1.0f, f/0.3*0.5);
    }
    if (f<0.6) {
        return HslColor((f-0.3)/0.3*0.1667, 1.0f, 0.5f);
    }
    return HslColor(0.166f, 1.0f, (f-0.6)/0.4f*0.5f+0.5f);
}

bool Fire::setGoodParams() {
    return true;
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
    static int flame = 0;
    static int center = 0;
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
    flame--;
    if (flame <= 0) {
        while (true) {
            int i = random(NUMPIXEL);
            if (params.posdata[i][2]<40) {
                center = i;
                flame = 10+random(15);
                break;
            }
        }
    }
    for (int i=0; i<NUMPIXEL; i++) {
        diff[i] = 0.0f;
        float d = vec_dist(params.posdata[center], params.posdata[i]);
        if (d<40) {
            diff[i]=0.08*(40.0-d)/40.0+temp[i]/20;
        }
    }
    // Burn 
    for (int i=0; i<NUMPIXEL; i++) {

        // spread around
        for (int r=0; r<4; r++) {
            if (params.nbrs[i][r]!=0xffff) {
                int j = params.nbrs[i][r];
                float d = (temp[j] - temp[i])/200;
                diff[i]+=d;
                diff[j]-=d;
             } else {
                diff[i]-=temp[i]/50+0.02;
             }
        }
        
        if (params.nbrs[i][4]!=0xffff) {
            int j = params.nbrs[i][4];
            float d = temp[i]*0.1;
            diff[i] -= d;
            diff[j] += d;
        } else {
            diff[i] -= temp[i]/20+0.02;
        }
        if (params.nbrs[i][5]!=0xffff) {
            int j = params.nbrs[i][5];
            float d = temp[j]*0.1;
            diff[i] += d;
            diff[j] -= d;
        } else {
            diff[i] -= temp[i]/20+0.02;
        }
        //diff[i] += temp[i]*fmap(params.posdata[i][2], params.mins[2], params.maxs[2], 0.2, -0.1);
    }
    for (int i=0; i<NUMPIXEL; i++) {
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
