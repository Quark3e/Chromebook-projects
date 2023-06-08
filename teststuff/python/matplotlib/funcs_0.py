#!/usr/bin/env python3

import matplotlib.pyplot as plt
from math import pow

def func0(x):
    return -9.237*10**-9*x**5 + 4.253*10**-6*x**4 - 0.0007235*x**3 + 0.05352*x**2 - 0.06534*x - 23.5

def func1(x):
    return 1.751*pow(10, -9)*pow(x, 5)-7.693*pow(10, -7)*pow(x, 4)+0.000117*pow(x, 3)-0.006447*pow(x, 2)+0.71*x+25.66;
    return 1.751*10**-9*x**5-7.693*10**-7*x**4+0.000117*x**3-0.006447*x**2+0.71*x+25.66

x = [n for n in range(0, 190, 10)]
y_0 = [func0(n) for n in x]
y_1 = [func1(n) for n in x]
y_1_fit = [func1(func0(n)) for n in x]

plt.plot(x, x, label="correct")
plt.plot(x, y_0, label="read poly")
plt.plot(x, y_1, label="\"solved\" poly (bare)")
plt.plot(x, y_1_fit, label="\"solved\" poly (correct)")

plt.grid()
plt.legend()

plt.show()
