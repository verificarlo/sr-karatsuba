#!/usr/bin/env python3

"""
Before running this script, please compile karatsuba.c with the following command
line:

verificarlo-c -O2 --function=karatsuba ./karatsuba.c -o karatsuba
"""


import math
import matplotlib.pyplot as plt
from matplotlib.pyplot import axis, cm
import numpy as np
import matplotlib
import subprocess

# SR unit-roundoff
u = 2**(-23)

# Lambda
lam = 0.1

# SR samples
sr_samples = 3

# polynomial degrees (the values are actually d+1)
d_values = [2**k for k in range(2, 17)]


def sr_run(d):
    """ compute the SR binary32 central-coefficient of the karatsuba product of two polynomials
    of degree d-2
    """

    cmd = 'VFC_BACKENDS_LOGFILE="verificarlo.log" '
    cmd += 'VFC_BACKENDS="libinterflop_mca_int.so --precision-binary32=24 --mode=rr" '
    cmd += './karatsuba {} {}'.format(d, sr_samples)
    samples = [float(s) for s in subprocess.getoutput(cmd).split()]
    # The first values are the reference computation in double,
    # and the condition number.
    return samples[0], samples[1], np.array(samples[2:])


def rn_run(n):
    """ compute the IEEE-754 RN binary32 central-coefficient of the karatsuba product of two polynomials
    of degree d-2
    """

    cmd = 'VFC_BACKENDS_LOGFILE="ieee.log" '
    cmd += 'VFC_BACKENDS="libinterflop_ieee.so" '
    cmd += './karatsuba {} {}'.format(d, 1)
    samples = [float(s) for s in subprocess.getoutput(cmd).split()]
    return np.array(samples[2])


def error(x, ref):
    """ returns relative forward error """
    return abs((x-ref)/ref)


# Define plot title and labels
title = "coefficients sampled uniformly in [0,1]"
plt.figure(title, figsize=(4.7, 4.7))
plt.suptitle(title)
plt.xlabel("$d+1$")
plt.xscale('log', base=2)
plt.yscale('log', base=2)
plt.ylabel("Error")
plt.ylim((2**-26, 2**10))
plt.grid(axis='y')

# RN-Binary32
ieee_error = []

# SR-Binary32
sr = [[] for _ in range(sr_samples)]
sr_average = []

# Bounds
b1 = []

# Compute all values and deterministic bounds across increasing n
for d in d_values:
    ref, K, samples = sr_run(d)
    print("d = {}, K= {}".format((d-2), K))
    ieee_value = rn_run(d)
    sr_average.append(error(np.mean(samples), ref))
    ieee_error.append(error(ieee_value, ref))

    for r in range(sr_samples):
        sr[r].append([error(samples[r], ref)])

    n = math.log(d, 2)-1
    b1.append(K*math.sqrt(u*((1+u)**(6*n)-1)*math.log(2.0/lam)))


# Plot the probabilistic bound on forward error
plt.plot(d_values, b1, linestyle=':', color='b',
         label=r'$K\sqrt{u\gamma_{6n}(u)}\sqrt{ln(2/\lambda)}$')

# Plot RN and SR samples
for r in range(sr_samples):
    plt.plot(d_values, sr[r], ' v', color='r',
             label='SR-nearness' if r == 0 else '')
plt.plot(d_values, ieee_error, ' *', color='y', label="RN-binary32")

plt.legend()
plt.savefig("positive.pdf", format='pdf')
plt.show()
