#include "../app.h"
#include "../utils.h"
#include "../params.h"
#include <vector>



class Jumping : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Jumping"; }
    virtual bool setGoodParams();
    virtual void stop();

    class Jumper;

    std::vector<Jumper*> jumpers;
};

bool Jumping::setGoodParams() {
    params.speed = 180;
    params.brightness = 255;
    return true;
}

class Jumping::Jumper {
    public:
    float t0;
    float tmul = 1.0;
    float* ctrlPoints;
    int color;
    Jumper(float t) : t0(t) {
        ctrlPoints = new float[9];
        float w = 2*PI*random(1000)/1000.0;
        float r = 50.0;
        ctrlPoints[0] = cos(w)*r;
        ctrlPoints[1] = sin(w)*r;
        ctrlPoints[2] = 0.0;
        w += PI+random(100)/100.0-0.5;
        ctrlPoints[6] = cos(w)*r;
        ctrlPoints[7] = sin(w)*r;
        ctrlPoints[8] = 0.0;
        float h = 2*(random(190)+30);
        ctrlPoints[3] = 0;
        ctrlPoints[4] = 0;
        ctrlPoints[5] = h;
        // s = 1/2 g t^2   <=> t = sqrt(2s/g);
        float ttot = 2*sqrt(2*h/100.0/9.81);
        tmul = 1/ttot;
        //Serial.printf("New Jumper with ttot=%.3f and tmul=%.3f\n", ttot, tmul);
        color = params.pixels->ColorHSV(random(0xffff),255,127);
    }
    ~Jumper() {
        delete[] ctrlPoints;
    }
    int getColor(float t, float* led) {
        float p[3];
        bezier2(p,(t-t0)*tmul,ctrlPoints);
        float d = vec_dist(p,led);
        if (d<15) {
            return color;
        }
        return scale(pow(0.5,(d-15)/2),color);
    }
    bool done(float t) {
        return (t-t0)*tmul>=1.0;
    }
};

void Jumping::stop() {
    while (jumpers.size()>0) {
        delete jumpers.back();
        jumpers.pop_back();
    }
}

void Jumping::loop() {
    static int speed=-1;
    if (speed!=params.speed) { // Handle speed change
        speed = params.speed;
        stop(); // Just delete current objects (if any)
    }
    float t = secs()/fmap(params.speed, 0, 255, 10, 1.0);
    // Generate jumpers
    while (jumpers.size()<5) {
        jumpers.push_back(new Jumper(t));
    }
    //unsigned long start = micros(); Takes ages !?!?
    for (int i=0; i<NUMPIXEL; i++) {
        int color = 0;
        for(auto const& jumper: jumpers) {
            color = color_add(color, jumper->getColor(t, params.posdata[i]));
        }
        params.pixels->setPixelColor(i, color);
    }
    //start = micros()-start;
    //Serial.printf("Jumping rendering: %ld us\n",start);
    // Remove finished jumpers
    for (auto it=jumpers.begin(); it!=jumpers.end();) {
        if ((*it)->done(t)) {
            delete *it;
            it = jumpers.erase(it);
        } else {
            it++;
        }
    }
    params.pixels->show();
}


// Instantiate (this will also register it into params)
Jumping jumping;
