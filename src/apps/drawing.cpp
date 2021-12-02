#include "../app.h"
#include "../utils.h"
#include "../params.h"

class Drawing : App {
    public:
    virtual void loop();
    virtual const char* buttonName() { return "Drawing"; }
};

void Drawing::loop() {
    const float r = 6;
    if (params.drawx || params.drawy) {
        float x = (200-params.drawx)/400.0*180;
        float z = (400-params.drawy)/400.0*180;
        params.drawx = 0;
        params.drawy = 0;
        Serial.printf("%.1f, %.1f\n", x,z);
        for (int i=0; i<NUMPIXEL; i++) {
            float zz = abs(z-params.posdata[i][2]);
            if (zz<r) {
                float xx = abs(x-params.posdata[i][0]);
                if (xx<r) {
                    xx = xx*xx+zz*zz;
                    if (xx<r*r) {
                        params.pixels->setPixelColor(i, params.color1);
                    }
                }
            }
        }
    }
    struct RGB {
        byte b;
        byte g;
        byte r;
    };
    // Fade (subtract one from each channel value)
    for (int i=0; i<NUMPIXEL; i++) {
        int color = params.pixels->getPixelColor(i);
        if (color) {
            RGB* c = (RGB*)&color;
            if (c->r) c->r--;
            if (c->g) c->g--;
            if (c->b) c->b--;
            params.pixels->setPixelColor(i, color);
        }
    }
    params.pixels->show();
}


Drawing drawing;