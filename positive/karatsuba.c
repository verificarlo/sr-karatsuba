#include <stdlib.h>

// initialize arrays with positive floating-points between 0 and 1
float *random_array(int l) {
  float *A = malloc(l * sizeof(float));
  for (int i = 0; i < l; i++) {
    A[i] = ((float)rand() / (float)RAND_MAX);
  }
  return A;
}

#include "../karatsuba.h"
