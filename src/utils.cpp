#include "params.h"

float fmap(float v, float fromMin, float fromMax, float toMin, float toMax) {
  return (v-fromMin)*(toMax-toMin)/(fromMax-fromMin)+toMin;
}

RgbColor color_mix(float t, RgbColor col1, RgbColor col2) {
  RgbColor c = {(uint8_t)((1-t)*col1.R+t*col2.R), (uint8_t)((1-t)*col1.G+t*col2.G), (uint8_t)((1-t)*col1.B+t*col2.B)};
  return c;
}

RgbColor mix(float f) {
  return color_mix(f, params.color1, params.color2);
}


/**
 * @brief Adds colors (clipping at 255);
 * 
 * @param c1 color1
 * @param c2 color2
 * @return int addition of c1 and c2
 */
int color_add(int col1, int col2) {
  int r = 0;
  for (int i=0; i<3; i++) {
    int c1 = col1 & (0xff << (8*i));
    int c2 = col2 & (0xff << (8*i));
    int c3 = c1+c2;
    if (c3>(0xff << (8*i))) {
      c3 = 0xff << (8*i);
    }
    r += c3 & (0xff<<(8*i));
  }
  return r;
}

RgbColor scale(float f, RgbColor color) {
  int r = color.R*f;
  if (r>255) r=255;
  int g = color.G*f;
  if (g>255) g=255;
  int b = color.B*f;
  if (b>255) b = 255;
  return RgbColor(r,g,b);
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
    dst[2] = src[2];
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
    dst[0] = src[0];
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
    dst[1] = src[1];
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

void vec_print(float *v) {
  Serial.printf("<%.2f, %.2f, %.2f>", v[0], v[1], v[2]);
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

float* bezier2(float* dst, float t, float* pts) {
    float c[3] = {(1-t)*(1-t), 2*(1-t)*t, t*t};
    for (int i=0; i<3; i++) {
        dst[i] = 0.0;
        for (int j=0; j<3; j++) {
            dst[i]+=c[j]*pts[3*j+i];
        }
    }
    return dst;
}


/**
 * @brief computes vector end-start
 * 
 * @param dst 
 * @param start 
 * @param end 
 * @return float* 
 */
float* vec_fromto(float *dst, float* start, float* end) {
    for (int i=0; i<3; i++) {
        dst[i] = end[i]-start[i];
    }
    return dst;
}

/**
 * @brief returns the sextant number: div 2 -> axis, mod 2 -> orientation.
 * 
 * @param center 
 * @param other 
 * @return int 
 */
int vec_sextant(float* center, float* other) {
    float v[3];
    vec_fromto(v, center, other);
    if (abs(v[0])<abs(v[1]) && abs(v[0])<abs(v[2])) {
        return v[0]>=0.0f ? 0 : 1;
    }
    if (abs(v[1])<abs(v[0]) && abs(v[1])<abs(v[2])) {
        return v[1]>=0.0f ? 2 : 3;
    }
    if (abs(v[2])<abs(v[0]) && abs(v[2])<abs(v[1])) {
        return v[2]>=0.0f ? 4 : 5;
    }
}
