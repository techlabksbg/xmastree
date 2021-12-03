#include "../app.h"
#include "../utils.h"
#include "../params.h"


class WireFrame : App {
    public:
    WireFrame();
    virtual void loop();
    virtual const char* buttonName() { return "Wireframe"; }
    //virtual bool setGoodParams();
    float** tetra;
    float** v;

};

WireFrame::WireFrame() {
    tetra = new float*[4] {
        new float[3] {1.0f, 1.0f, 1.0f},
        new float[3] {-1.0, -1.0f, 1.0f},
        new float[3] {1.0f, -1.0f, -1.0f},
        new float[3] {-1.0f, 1.0f, -1.0f}
    };
    v = new float*[4];
    for (int i=0;i<4;i++) { v[i] = new float[3];}
}

void WireFrame::loop() {
    float t = secs()/fmap(params.speed, 0, 255, 20, 1.0);
    float wxy = t/3;
    float wzx = t;
    
    float* trans = new float[3] {0.0f, 0.0f, 90.0f};


    /*float* test = new float[3] {8.0f, 3.0f, 7.0f};
    float lambda = line_project(test, tetra[0], tetra[1]);
    vec_print(test);
    Serial.printf(" -> lambda=%.2f\n",lambda);
    lambda = dist_to_segment(test, tetra[0], tetra[1]);
    Serial.printf(" -> dist=%.2f\n",lambda);
    delete[] test;
    return; */

    for (int i=0;i<4;i++) { 
        vec_copy(v[i], tetra[i]);
        vec_rotzx(v[i], tetra[i], wzx);
        vec_rotxy(v[i], v[i], wxy);
        vec_mul(v[i],50.0/2); // 80cm cube
        vec_add(v[i], trans);
        //vec_print(v[i]);
    }
    //Serial.print("[");
    //uint64_t bits = 0;
    for (int led=0; led<NUMPIXEL; led++) {
        /*if (led%64==0) {
            if (led>0) {
                Serial.printf("0x%llx,",bits);
            }
            bits=0;
        }*/
        float d = 1000.0;
        int best = -1;
        //Serial.print("dists = ");
        int n = 0;
        for (int p1=0; p1<4; p1++) {
            for (int p2=p1+1; p2<4; p2++) {
                float l = dist_to_segment(params.posdata[led], v[p1], v[p2]);
                /*if (led % 57 == 0) {
                    Serial.print(l);
                    Serial.print(',');
                }*/
                if (l<d) {
                    d=l;
                    best = n;
                }
                n++;
            }
        }
        float hue = fmod(best/6.0,1.0);
        // 8cm tubes
        if (d<10.0) {
            float v = 0.5;
            if (d>6.0) {
                v = (10-d)/8.0;
            }
            params.pixels->SetPixelColor(led, HslColor(hue, 1.0f, v));
        } else {
            params.pixels->SetPixelColor(led, 0);
        }
        
        /*if (d<15.0) {
            bits |= (((uint64_t)1) << (led%64));
        }*/
    }
    //Serial.printf("0x%llx],\\\n",bits);
    params.pixels->Show();
}


// Instantiate (this will also register it into params)
WireFrame wireframe;