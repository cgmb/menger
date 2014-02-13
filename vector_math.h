#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

#include <math.h>
#include <string.h>

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

void translate_v3fo(float x, float y, float z, const float* v, float* result) {
  result[0] = v[0] + x;
  result[1] = v[1] + y;
  result[2] = v[2] + z;
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

void m4f_copy_m4fo(const float* m, float* result) {
  memcpy(result, m, 16 * sizeof(float));
}

void m4f_mul_m4fo(const float* m, const float* n, float* result) {
  result[ 0] = m[ 0]*n[ 0] + m[ 1]*n[ 4] + m[ 2]*n[ 8] + m[ 3]*n[12];
  result[ 1] = m[ 0]*n[ 1] + m[ 1]*n[ 5] + m[ 2]*n[ 9] + m[ 3]*n[13];
  result[ 2] = m[ 0]*n[ 2] + m[ 1]*n[ 6] + m[ 2]*n[10] + m[ 3]*n[14];
  result[ 3] = m[ 0]*n[ 3] + m[ 1]*n[ 7] + m[ 2]*n[11] + m[ 3]*n[15];

  result[ 4] = m[ 4]*n[ 0] + m[ 5]*n[ 4] + m[ 6]*n[ 8] + m[ 7]*n[12];
  result[ 5] = m[ 4]*n[ 1] + m[ 5]*n[ 5] + m[ 6]*n[ 9] + m[ 7]*n[13];
  result[ 6] = m[ 4]*n[ 2] + m[ 5]*n[ 6] + m[ 6]*n[10] + m[ 7]*n[14];
  result[ 7] = m[ 4]*n[ 3] + m[ 5]*n[ 7] + m[ 6]*n[11] + m[ 7]*n[15];

  result[ 8] = m[ 8]*n[ 0] + m[ 9]*n[ 4] + m[10]*n[ 8] + m[11]*n[12];
  result[ 9] = m[ 8]*n[ 1] + m[ 9]*n[ 5] + m[10]*n[ 9] + m[11]*n[13];
  result[10] = m[ 8]*n[ 2] + m[ 9]*n[ 6] + m[10]*n[10] + m[11]*n[14];
  result[11] = m[ 8]*n[ 3] + m[ 9]*n[ 7] + m[10]*n[11] + m[11]*n[15];

  result[12] = m[12]*n[ 0] + m[13]*n[ 4] + m[14]*n[ 8] + m[15]*n[12];
  result[13] = m[12]*n[ 1] + m[13]*n[ 5] + m[14]*n[ 9] + m[15]*n[13];
  result[14] = m[12]*n[ 2] + m[13]*n[ 6] + m[14]*n[10] + m[15]*n[14];
  result[15] = m[12]*n[ 3] + m[13]*n[ 7] + m[14]*n[11] + m[15]*n[15];
}

void m4f_mul_m4f(const float* in, float* in_out) {
  float temp[16];
  m4f_mul_m4fo(in, in_out, temp);
  m4f_copy_m4fo(temp, in_out);
}

void m4f_mul_v4fo(const float* m, const float* v, float* result) {
  result[0] = m[ 0]*v[0] + m[ 1]*v[1] + m[ 2]*v[2] + m[ 3]*v[3];
  result[1] = m[ 4]*v[0] + m[ 5]*v[1] + m[ 6]*v[2] + m[ 7]*v[3];
  result[2] = m[ 8]*v[0] + m[ 9]*v[1] + m[10]*v[2] + m[11]*v[3];
  result[3] = m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3];
}

void m4f_fill_rotx_m4fo(float rx, float* result) {
  float m[] = {
    1,  0,         0,        0,
    0,  cosf(rx), -sinf(rx), 0,
    0,  sinf(rx),  cosf(rx), 0,
    0,  0,         0,        1,
  };
  m4f_copy_m4fo(m, result);
}

void m4f_fill_roty_m4fo(float ry, float* result) {
  float m[] = {
    cosf(ry),  0,  sinf(ry), 0,
    0,         1,  0,        0,
   -sinf(ry),  0,  cosf(ry), 0,
    0,         0,  0,        1,
  };
  m4f_copy_m4fo(m, result);
}

void m4f_fill_rotz_m4fo(float rz, float* result) {
  float m[] = {
    cosf(rz), -sinf(rz), 0,  0,
    sinf(rz),  cosf(rz), 0,  0,
    0,         0,        1,  0,
    0,         0,        0,  1,
  };
  m4f_copy_m4fo(m, result);
}

void rotatex_v4fo(float rx, const float* v, float* result) {
  float rm[16];
  m4f_fill_rotx_m4fo(rx, rm);
  m4f_mul_m4fo(rm, v, result);
}

void rotatey_v4fo(float ry, const float* v, float* result) {
  float rm[16];
  m4f_fill_roty_m4fo(ry, rm);
  m4f_mul_m4fo(rm, v, result);
}

void m4f_fill_scale_m4fo(float sx, float sy, float sz, float* result) {
  float m[] = {
    sx, 0,  0,  0,
    0,  sy, 0,  0,
    0,  0,  sz, 0,
    0,  0,  0,  1,
  };
  m4f_copy_m4fo(m, result);
}

void m4f_fill_trans_m4fo(float x, float y, float z, float* result) {
  float m[] = {
    1, 0, 0, x,
    0, 1, 0, y,
    0, 0, 1, z,
    0, 0, 0, 1,
  };
  m4f_copy_m4fo(m, result);
}

#endif
