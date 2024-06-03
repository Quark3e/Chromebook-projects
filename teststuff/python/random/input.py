#!/usr/bin/env python3

import sys

def testFunc(x):
    for i in range(len(x)):
        x[i] = 2*x[i]
    return x

var1, var2, var3 = 0, 0, 0


var3 = input("Enter three values: ").split()
print(len(var3))
print(var3)
if var3[0] == 'exit':
    print("detected \"exit\", is exiting...")
    sys.exit()
else:
    var3 = int(var3)

if len(var3) == 3:
    test1 = 2*float(var3[0])
    test2 = 2*float(var3[1])
    test3 = 2*float(var3[2])
    # test1, test2, test3 = testFunc(var3)
    print(test1, test2, test3)

print("\n1:", var1, "\n2:", var2, "\n3:", var3[0])
