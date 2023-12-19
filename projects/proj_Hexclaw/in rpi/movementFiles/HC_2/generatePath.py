#!/usr/bin/env python3

import math
import os.path


absPath = str(os.path.realpath(__file__)[:len(str(os.path.realpath(__file__)))-len("generatePath.py")])

fileName = "coord_3.dat"
type = "coord"
opt2 = "True"
mode = 2

comments = {
    3: "#if TotalTime (\";tt\") is used then the line (or atleast the value) has to end with \";\"\n",
    4: "#movement command syntax/structure: #{x}:{y}:{z};{alpha}:{beta}:{gamma};tt{TotalTime};\n"
}


line = 0
f = open(absPath+fileName, "a")
f.write(f"type:{type}:{opt2}\n")
line+=1
f.write(f"mode:{mode}\n")
line+=1

f.write(comments[3])
f.write(comments[4])

f.write(f"sleep:{2}\n")

for i in range(0, 151, 10):
    f.write(f"{-i}:150:150;0:{-round(90-90*i/150)}:0;tt{0.1};\n")
    line+=1
    if i==0: f.write(f"sleep:3\n")
    f.write(f"sleep:{0.2}\n")
for i in range(150, -10, -10):
    f.write(f"{-i}:150:150;0:{-round(90-90*i/150)}:0;tt{0.1};\n")
    line+=1
    f.write(f"sleep:{0.2}\n")

f.write(f"sleep:{2}\n")

for i in range(0, 151, 10):
    f.write(f"{-i}:150:150;0:{-round(90-45*i/150)}:0;tt{0.1};\n")
    line+=1
    f.write(f"sleep:{0.2}\n")
for i in range(150, -10, -10):
    f.write(f"{-i}:150:150;0:{-round(90-45*i/150)}:0;tt{0.1};\n")
    line+=1
    f.write(f"sleep:{0.15}\n")

f.write("\n")
f.close()
