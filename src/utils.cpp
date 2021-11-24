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

float* vec_add_mull(float *dst, float *src, float mul) {
   for (int i=0; i<3; i++) {
    dst[i]+= src[i]*mul;
  }
  return dst;
}

float* vec_mul(float* dst, float mul) {
  for (int i=0; i<3; i++) {
    dst[i] *= mul;
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

float vec_dist2(float* a, float* b) {
  float l = 0.0;
  for (int i=0; i<3; i++) {
    l+=(a[i]-b[i])*(a[i]-b[i]);
  }
  return l;
}

float vec_dist(float* a, float* b) {
  return sqrt(vec_dist2(a,b));
}

float vec_dot(float *src1, float *src2) {
  float d = 0.0;
  for (int i=0; i<3; i++) {
    d+=src1[i]*src2[i];
  }
  return d;
}

float* vec_normalize(float* vec) {
  return vec_mul(vec, 1.0/vec_norm(vec));
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

float* vec_rotyz(float *dst, float *src, float w) {
  float c = cos(w);
  float s = sin(w);
  if (dst==src) {
    float x = c*src[1]-s*src[2];
    dst[2] = s*src[1]+c*src[2];
    dst[1] = x;
  } else {
    dst[1] = c*src[1]-s*src[2];
    dst[2] = s*src[1]+c*src[2];
  }
  return dst;
}

float* vec_rotzx(float *dst, float *src, float w) {
  float c = cos(w);
  float s = sin(w);
  if (dst==src) {
    float x = c*src[2]-s*src[0];
    dst[0] = s*src[2]+c*src[0];
    dst[2] = x;
  } else {
    dst[2] = c*src[2]-s*src[0];
    dst[0] = s*src[2]+c*src[0];
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

float line_project(float* pt, float* linea, float* lineb) {
  float v[3];
  // pt2-pt1 
  vec_add_mull(vec_copy(v, lineb), linea, -1);
  // (v*pt-v*a)/(v*v)
  return (vec_dot(v,pt)-vec_dot(v,linea))/vec_norm2(v);
}

float dist_to_segment(float* pt, float* sega, float* segb) {
  float t = line_project(pt, sega, segb);
  if (t<0.0) {
    return vec_dist(pt, sega);
  } else if (t>1.0) {
    return vec_dist(pt, segb);
  }
  float v[3];
  // v = a*(1-t)+b*t
  vec_mul(vec_copy(v,segb),t);
  vec_add_mull(v, sega, 1.0-t);
  return vec_dist(v,pt);
}