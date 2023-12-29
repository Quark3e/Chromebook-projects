#!/usr/bin/env python3

import math
import numpy as np

toRadians = lambda degree: degree*np.pi/180
getPosNeg = lambda val: val/abs(val)

def findClosest(pos_given, pos_closest, r=320):
    if None in pos_given:
        print("findClosest: None in pos_[]")
        for i in range(len(pos_given)): pos_closest[i]=pos_given[i]
        return
    if math.sqrt(given_pos[0]**2+given_pos[1]**2+given_pos[2]**2)<=r:
        print("findClosest: input pos is already within work-envelope")
        for i in range(len(pos_given)): pos_closest[i] = pos_given[i]
        return
    alpha, beta = 0, 0
    if pos_given[1]>0: alpha = math.atan(pos_given[0]/pos_given[1])
    elif pos_given[1]==0:
        if pos_given[0]>0: alpha = toRadians(90)
        elif pos_given[0]<0: alpha = toRadians(-90)
        elif pos_given[0]==0: alpha = None
    elif pos_given[1]<0:
        alpha = math.atan(pos_given[0]/pos_given[1])+toRadians(180)*getPosNeg(pos_given[0])

    if pos_given[0]**2+pos_given[1]**2>0:
        beta = math.atan(pos_given[2]/math.sqrt(pos_given[0]**2+pos_given[1]**2))
    else:
        beta = None
    if None in [alpha, beta]:
        print("findClosest: pos_given[0]**2+pos_given[1]**2 == 0")
        return

    pos_closest[0] = math.cos(beta)*math.sin(alpha)*r
    pos_closest[1] = math.cos(beta)*math.cos(alpha)*r
    pos_closest[2] = math.sin(beta)*r


r = 320

if __name__=="__main__":
    while True:
        given_pos = [0, 0, 0]
        closest_pos = [-1, -1, -1]
        print("----------")
        inp = input(f"r = {r} \nenter input coordinances: ").split()
        if inp[0]=="exit": break
        if len(inp)==3:
            given_pos = [float(i) for i in inp]
        else:
            print("\nfindClosest: incorrect input")
            continue

        findClosest(given_pos, closest_pos)
        print("")
        print("given_pos   :", [round(i,2) for i in given_pos])
        print("closest_pos :", [round(i,2) for i in closest_pos])
