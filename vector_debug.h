#ifndef VECTOR_DEBUG_H
#define VECTOR_DEBUG_H

#include "vector_math.h"

void m4f_print(const float* m) {
  printf("[% f % f % f % f]\n"
         "[% f % f % f % f]\n"
         "[% f % f % f % f]\n"
         "[% f % f % f % f]\n",
    m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7],
    m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
}

void print_indexes_glub(const GLubyte* begin, size_t count, size_t row_length) {
  size_t i;
  for (i = 0; i < count; ++i) {
    printf("%2u, ", begin[i]);
    if ((i + 1ul) % row_length == 0) {
      printf("\n");
    }
  }
}

void print_av3f(const float* begin, size_t count, size_t row_length) {
  size_t i;
  for (i = 0; i < count; ++i) {
    printf("% f, ", begin[i]);
    if ((i + 1ul) % row_length == 0) {
      printf("\n");
    }
  }
}

#endif