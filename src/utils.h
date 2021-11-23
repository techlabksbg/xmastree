
/**
 * @brief Linear mapping from one interval to another
 * 
 * @param v Value to be mapped
 * @param fromMin source interval lower bound
 * @param fromMax source interval upper bound
 * @param toMin destination intervall lower bound (might as well be upper)
 * @param toMax destination intervall upper bound (might as well be lower)
 * @return float Mapped value
 */
float fmap(float v, float fromMin, float fromMax, float toMin, float toMax);

/**
 * @brief Linear RGB interpolation of params.color1 and params.color2
 * 
 * @param f 0 means color1, 1 means color2
 * @return int interpolated color value
 */
int mix(float f);

/**
 * @brief RGB-scaling of a color (basically dimming it)
 * 
 * @param f Scale factor (typically in [0,1])
 * @param color Color to be scaled
 * @return int Scaled color
 */
int scale(float f, int color);

/**
 * @brief Copies a vector from src to dest
 * 
 * @param dst destination
 * @param src source
 * @return float* the destination
 */
float* vec_copy(float *dst, float *src);

/**
 * @brief Add src to dst (and changes it)
 * 
 * @param dst destination (result)
 * @param src vector to add
 * @return float* the destination (result)
 */
float* vec_add(float *dst, float *src);

/**
 * @brief dst=dst+f*src Add a multiple of a vector to another.
 * 
 * @param dst destination (result)
 * @param src source vector
 * @param mul scale factor for source vector
 * @return float* 
 */
float* vec_add_mull(float *dst, float *src, float mul);

/**
 * @brief Scales a vector (modifies it)
 * 
 * @param dst vector to be scaled
 * @param mul scale factor
 * @return float* scaled vector
 */
float* vec_mul(float* dst, float mul);

/**
 * @brief Compute the square of the norm of a vector
 * 
 * @param src vector to compute the squared norm of
 * @return float norm squared
 */
float vec_norm2(float *src);

/**
 * @brief Compute the norm of a vector
 * 
 * @param src input vector
 * @return float norm of vector
 */
float vec_norm(float *src);

/**
 * @brief Distance squared between two points
 * 
 * @param a 1st point
 * @param b 2nd point
 * @return float squared distance 
 */
float vec_dist2(float* a, float* b);

/**
 * @brief Distance between two points
 * 
 * @param a 1st point
 * @param b 2nd point
 * @return float 
 */
float vec_dist(float* a, float* b);

/**
 * @brief Normalize a vector
 * 
 * @param vec vector to be normalized
 * @return float* normalized vector
 */
float* vec_normalize(float* vec);


/**
 * @brief Compute the dot product
 * 
 * @param src1 1st vector
 * @param src2 2nd vector
 * @return float dot product
 */
float vec_dot(float *src1, float *src2);

/**
 * @brief Rotates a vector around z by an angle w
 * 
 * @param dst resulting vector (will be overwritten)
 * @param src source vector (may be the same as dst)
 * @param w rotation angle (radians)
 * @return float* dst (resulting vector)
 */
float* vec_rotxy(float *dst, float *src, float w);

/**
 * @brief cross product. 
 * 
 * @param dst result vector **Must** be different from src1 and src2
 * @param src1 1st vector of cross product
 * @param src2 2nd vector of cross product
 * @return float* 
 */
float* vec_cross(float* dst, float* src1, float* src2);

/**
 * @brief Project a point onto a line
 * 
 * @param pt Point to projected
 * @param linea 1st point on line
 * @param lineb 2nd point on line
 * @return float parameter t such that pt=(1-t)*linea+t*lineb
 */
float line_project(float* pt, float* linea, float* lineb);

/**
 * @brief Distance to a segment (not the entire line, so «it makes round caps»
 * 
 * @param pt Point for distance
 * @param sega Start of segment
 * @param segb End of segment
 * @return float Distance to the segment
 */
float dist_to_segment(float* pt, float* sega, float* segb);