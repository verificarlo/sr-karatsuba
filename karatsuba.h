#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// compute the Karatsuba Polynomial product for the central coefficient of R
// A,B are coefficients arrays of size l (which must be a power of 2)
//
//
// Three different functions are used:
//  - karatsuba_reference performs the computation in binary64 (double) and
//  serves as the reference;
//  - karatsuba performs the computation in binary32 (float), depending on the
//  instrumentation, it will compute the RN32 or SR32 values;
//  - karatsuba_condition helps computing the condition number; in fact it
//  computes K*|C|.

void karatsuba_reference(int l, double *C, double *A, double *B) {
  if (l == 1) {
    *C = A[0] * B[0];
  } else {
    int h = l / 2;
    double P0, P1, P2;
    double CA[h], CB[h];

    for (int i = 0; i < h; i++) {
      CA[i] = A[i] - A[i + h];
      CB[i] = B[i + h] - B[i];
    }

    karatsuba_reference(h, &P0, A, B);
    karatsuba_reference(h, &P2, A + h, B + h);
    karatsuba_reference(h, &P1, CA, CB);

    *C = P1 + (P0 + P2);
  }
}

void karatsuba(int l, float *C, float *A, float *B) {
  if (l == 1) {
    *C = A[0] * B[0];
  } else {
    int h = l / 2;
    float P0, P1, P2;
    float CA[h], CB[h];

    for (int i = 0; i < h; i++) {
      CA[i] = A[i] - A[i + h];
      CB[i] = B[i + h] - B[i];
    }

    karatsuba(h, &P0, A, B);
    karatsuba(h, &P2, A + h, B + h);
    karatsuba(h, &P1, CA, CB);

    *C = P1 + (P0 + P2);
  }
}

void karatsuba_condition(int l, double *C, double *A, double *B) {
  if (l == 1) {
    *C = fabs(A[0] * B[0]);
  } else {
    int h = l / 2;
    double P0, P1, P2;
    double CA[h], CB[h];

    for (int i = 0; i < h; i++) {
      CA[i] = fabs(A[i]) + fabs(A[i + h]);
      CB[i] = fabs(B[i + h]) + fabs(B[i]);
    }

    karatsuba_condition(h, &P0, A, B);
    karatsuba_condition(h, &P2, A + h, B + h);
    karatsuba_condition(h, &P1, CA, CB);

    *C = fabs(P1) + (fabs(P0) + fabs(P2));
  }
}

// short sanity tests
void test() {
  double A[] = {1., 2., 0., 5.};
  double B[] = {4., 3., 2., 3.};
  double C;

  karatsuba_reference(1, &C, A, B);
  assert(fabs(C - 4.0) < 1e-6);

  karatsuba_reference(2, &C, A, B);
  assert(fabs(C - 11.0) < 1e-6);

  karatsuba_reference(4, &C, A, B);
  assert(fabs(C - 27.0) < 1e-6);
}

int main(int argc, char *argv[]) {

  // run tests
  test();

  // parse arguments: d+2, repetitions
  assert(argc == 3 && "usage: ./karatsuba <d+2> <repetitions>");

  // deg is the degree of the resulting polynomial + 2
  long deg = strtol(argv[1], NULL, 10);

  // compute the size of the input polynomials
  long l = deg / 2;

  long repetitions = strtol(argv[2], NULL, 10);

  // seed RNG with fixed seed 0
  srand(0);

  // generate random input arrays in float
  float *U = random_array(l);
  float *V = random_array(l);

  // copy values to double arrays
  double *UD = malloc(l * sizeof(double));
  double *VD = malloc(l * sizeof(double));
  for (int i = 0; i < l; i++) {
    UD[i] = U[i];
    VD[i] = V[i];
  }

  // compute reference and condition number
  double ref, cond;
  karatsuba_reference(l, &ref, UD, VD);
  printf("%.17lf\n", ref);
  karatsuba_condition(l, &cond, UD, VD);
  printf("%.17lf\n", (cond / fabs(ref)));

  // compute 'repetitions' samples with binary32
  float res;
  for (int r = 0; r < repetitions; r++) {
    karatsuba(l, &res, U, V);
    printf("%.17lf\n", (double)res);
  }

  // free memory
  free(U);
  free(UD);
  free(V);
  free(VD);
  return (0);
}
