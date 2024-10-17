#include <stdlib.h>

// initialize arrays with floating-points between -0.5 and 0.5
float *random_array(int l) {
  float *A = malloc(l * sizeof(float));
  for (int i = 0; i < l; i++) {
    A[i] = ((float)rand() / (float)RAND_MAX) - 0.5;
  }
  return A;
}

#include "../karatsuba.h"
