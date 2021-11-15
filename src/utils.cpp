#include "params.h"

float fmap(float v, float fromMin, float fromMax, float toMin, float toMax) {
  return (v-fromMin)*(toMax-toMin)/(fromMax-fromMin)+toMin;
}


int mix(float f) {
  int r = 0;
  for (int i=0; i<3; i++) {
    int c1 = params.color1 & (0xff << (8*i));
    int c2 = params.color2 & (0xff << (8*i));
    int c3 = (1.0-f)*c1+f*c2;
    r += c3 & (0xff<<(8*i));
  }
  //Serial.printf("mix %f of color1=%06x and color2=%06x results in %06x\n", f, color1, color2, r);
  return r;
}

int scale(float f, int color) {
  int r = 0;
  for (int i=0; i<3; i++) {
    r |= ((int)(f*(color & (0xff<<(8*i))))) & (0xff<<(8*i));
  }
  return r;
}