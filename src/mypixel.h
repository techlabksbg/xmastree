#pragma once
#include <NeoPixelBus.h>

#define PIN0 13
#define PIN1 12


// See https://github.com/Makuna/NeoPixelBus/wiki/ESP32-NeoMethods  (NeoEsp32BitBang800KbpsMethod works, but...)
#define NEO0 NeoPixelBus<NeoRgbFeature, NeoEsp32Rmt0800KbpsMethod>
#define NEO1 NeoPixelBus<NeoRgbFeature, NeoEsp32Rmt1800KbpsMethod>

class MyPixel {
    NEO0* pixel0;
    NEO1* pixel1;

public:
    void Begin();    
    void Show();
    void SetPixelColor(int pixel, RgbColor color);
    void ClearTo(RgbColor color);
};

