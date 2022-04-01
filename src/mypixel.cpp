#include "mypixel.h"

void MyPixel::Begin() {
    pixel0 = new NEO0(250, PIN0);
    pixel1 = new NEO1(250, PIN1);
    pixel0->Begin();
    pixel1->Begin();
}

void MyPixel::Show() {
    pixel0->Show();
    pixel1->Show();
}

void MyPixel::SetPixelColor(int pixel, RgbColor color) {
    if (pixel<250) {
        pixel0->SetPixelColor(pixel, color);
    } else {
        pixel1->SetPixelColor(pixel-250, color);
    }
}

void MyPixel::ClearTo(RgbColor color) {
    pixel0->ClearTo(color);
    pixel1->ClearTo(color);
}

