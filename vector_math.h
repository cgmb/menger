#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

void cross_product_3fo(const float* x, const float* y, float* result) {
  result[0] = x[1]*y[2] - x[2]*y[1];
  result[1] = x[2]*y[0] - x[0]*y[2];
  result[2] = x[0]*y[1] - x[1]*y[0];
}

float magnitude_3f(const float* x) {
  return sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
}

void scale_3f(float scalar, float* x) {
  x[0] *= scalar;
  x[1] *= scalar;
  x[2] *= scalar;
}

void difference_3fo(const float* x, const float* y, float* result) {
  result[0] = x[0] - y[0];
  result[1] = x[1] - y[1];
  result[2] = x[2] - y[2];
}

void normalize_3f(float* x) {
  float magnitude = magnitude_3f(x);
  if (magnitude == 0.f) {
    return; // not really sure what to do about this...
  }
  scale_3f(1/magnitude, x);
}

void matrix3_mul_3fo(const float* m, const float* v, float* result) {
  result[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2];
  result[1] = m[3]*v[0] + m[4]*v[1] + m[5]*v[2];
  result[2] = m[6]*v[0] + m[7]*v[1] + m[8]*v[2];
}

void rotatex_3fo(float rx, const float* v, float* result) {
  float m[] = {
    1,  0,         0,
    0,  cosf(rx), -sinf(rx),
    0,  sinf(rx),  cosf(rx),
  };
  matrix3_mul_3fo(m, v, result);
}

void rotatey_3fo(float ry, const float* v, float* result) {
  float m[] = {
    cosf(ry),  0,  sinf(ry),
    0,         1,  0,
   -sinf(ry),  0,  cosf(ry),
  };
  matrix3_mul_3fo(m, v, result);
}

#endif
