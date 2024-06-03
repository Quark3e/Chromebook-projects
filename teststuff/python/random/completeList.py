#!/usr/bin/env python3

import numpy as np

def completeList_2d(inpList, inpLen):
    nPolys = [\
        np.poly1d(np.polyfit([round(n/len(inpList)*inpLen) for n in range(len(inpList))], [n[0] for n in inpList], 20)),
        np.poly1d(np.polyfit([round(n/len(inpList)*inpLen) for n in range(len(inpList))], [n[1] for n in inpList], 20)),
    ]
    returArr = [None for _ in range(inpLen)]
    for n in range(inpLen):
        returArr[n] = [round(nPolys[0](n)),round(nPolys[1](n))]

    return returArr

temp0 = [[n,99-n] for n in range(100) if n%3==0]

print(len(temp0), ": ", temp0)

print("------")
temp1 = completeList_2d(temp0, 100)
print("------")

print(len(temp1),": ", temp1)

print("------")

cnt = 0
for i in range(100):
    print(i, cnt)
    print(temp1[i])
    if i%3==0:
        print(temp0[cnt])
        cnt+=1
    else: print("")

    print("")
