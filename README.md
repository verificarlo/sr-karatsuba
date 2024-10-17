# sr-variance-bounds

Scripts to reproduce the numerical experiments of the paper "Error Functions and Martingales in Stochastic Rounding".

Before running the scripts, ensure that you have installed [Verificarlo v2.0.0](https://github.com/verificarlo/verificarlo/releases/tag/v2.0.0), Python 3, and matplotlib on your computer.

## Karatsuba polynomial multiplication

To reproduce Karatsuba numerical experiments use the following commands.
For input coefficients randomly (uniform distribution) sampled over `[0.0, 1.0]`:

```bash
sr-karatsuba$ cd positive
sr-karatsuba/positive$ verificarlo-c -O2 --function=karatsuba ./karatsuba.c -o karatsuba
sr-karatsuba/positive$ ./run-and-plot.py
```

For input coefficients randomly (uniform distribution) sampled over `[-0.5, 0.5]`, redo the above steps in the `centered` directory.
