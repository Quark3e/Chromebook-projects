#!/usr/bin/env python3

#DOES NOT WORK

import matplotlib.pyplot as plt
import numpy as np
import math
import os

a1, a3, b3, m, c = 5*[0]
k = 0
P0 = [0, 0]
P1 = [0.1, 0.15]
P2 = [0.9, 0.85]
P3 = [1, 1]

def define_k():
    global k
    k = ((P2[1] - P1[1]) / (P2[0] - P1[0]))
def define_a1():
    global a1
    a1 = (k / (2*P1[0]))
def define_a3():
    global a3
    a3 = (k / (2*(P2[0] - 1)))
def define_b3():
    global b3
    b3 = (0 - k / (P2[0] + 1))
def define_m():
    global m
    m = (P2[1] - k*P2[0])
    
def define_c():
    global c
    c = (P2[1] - a3*pow(P2[0],2) - b3*P2[0])

def func_1(x):
    return (pow(x,2)*a1)
def func_2(x):
    return (k*x + m)
def func_3(x):
    return (a3*pow(x,2) + b3*x + c)


def main():
    define_k()
    define_a1()
    define_a3()
    define_b3()
    define_m()
    define_c()
    print("\n k:",k,"\n a1:",a1,"\n a3:",a3,"\n b3:",b3,"\n m:",m,"\n c:",c,sep='')
    
    x = np.linspace(0,1,100)
    y = 100*[0]
    for i in range(100):
        if i < P1[0]*100: y[i] = func_1(x[i])
        elif i >= P1[0]*100 and i < P2[0]*100: y[i] = func_2(x[i])
        elif i >= P2[0]*100: y[i] = func_3(x[i])
    
    fig = plt.figure()
    ax = fig.add_subplot(1,1,1)
    plt.plot(x,y,'r')
    plt.show()


if __name__ == "__main__":
    main()
