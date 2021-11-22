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



// Simple vector library, acting soley on float[3] objects

/**
 * Copy from src to dst
 * @param src Source
 * @param dst Destination
 * @returns dst (Pointer to result)
 */
float* vec_copy(float *dst, float *src) {
  for (int i=0; i<3; i++) {
    dst[i] = src[i];
  }
  return dst;
}

float* vec_add(float *dst, float *src) {
   for (int i=0; i<3; i++) {
    dst[i]+= src[i];
  }
  return dst;
}

float* vec_add_null(float *dst, float *src, float mul) {
   for (int i=0; i<3; i++) {
    dst[i]+= src[i]*mul;
  }
  return dst;
}

float vec_norm2(float *src) {
  float l = 0.0;
  for (int i=0; i<3; i++) {
    l+=src[i]*src[i];
  }
  return l;
}

float vec_norm(float *src) {
  return sqrt(vec_norm2(src));
}

float vec_dot(float *src1, float *src2) {
  float d = 0.0;
  for (int i=0; i<3; i++) {
    d+=src1[i]*src2[i];
  }
  return d;
}


float* vec_rotxy(float *dst, float *src, float w) {
  float c = cos(w);
  float s = sin(w);
  if (dst==src) {
    float x = c*src[0]-s*src[1];
    dst[1] = s*src[0]+c*src[1];
    dst[0] = x;
  } else {
    dst[0] = c*src[0]-s*src[1];
    dst[1] = s*src[0]+c*src[1];
  }
  return dst;
}

/**
 * Cross product. Destination must be different from src1 and src2
 * @param src1 1st vector
 * @param src2 2nd vector
 * @param dst result vector
 * @returns result vector
 */
float* vec_cross(float* dst, float* src1, float* src2) {
  dst[0] = src1[1]*src2[2]-src1[2]*src2[1];
  dst[1] = src1[2]*src2[0]-src1[0]*src2[2];
  dst[2] = src1[0]*src2[1]-src1[1]*src1[0];
  return dst;  
}