#!/usr/bin/env python3

import math
import os.path


absPath = str(os.path.realpath(__file__)[:len(str(os.path.realpath(__file__)))-len("generatePath.py")])

fileName = "coord_5.dat"
type = "coord"
opt2 = "True"
mode = 2

comments = [
    "#if TotalTime (\";tt\") is used then the line (or atleast the value) has to end with \";\"",
    "#movement command syntax/structure: #{x}:{y}:{z};{alpha}:{beta}:{gamma};tt{TotalTime};",
    "Spiraling across line"
]
for i in range(len(comments)): comments[i]+="\n"


line = 0
f = open(absPath+fileName, "a")
f.write(f"type:{type}:{opt2}\n")
line+=1
f.write(f"mode:{mode}\n")
line+=1

for i in comments: f.write(i)

f.write("0:200:150;0:0:0;tt1;\n")
f.write("sleep:0.5\n")
f.write("0:200:150;0:-90:0;tt1;\n")
f.write("sleep:0.5\n")




f.write("\n")
f.close()
