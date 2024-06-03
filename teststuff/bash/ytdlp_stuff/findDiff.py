#!/usr/bin/env python3

import string
import sys
import os

os.system("exiftool original.m4a > compare.txt")
f = open("compare.txt", "a")
f.write("----\n")
f.close()
os.system("exiftool original.m4a >> compare.txt")

letters = string.ascii_lowercase + string.ascii_uppercase + " -"

f = open("compare.txt")

def lastCons(text):
    count=0
    spaceCount = 0
    for x in text:
        if x==" ": spaceCount+=1
        else: spaceCount=0
        if spaceCount>=2: return (count-1)
        if x in letters: count+=1
    return count

l1 = []
l2 = []
varLists = []
varLists.append([])

for line in f:
    line = line[:lastCons(line[:line.find(":")])]
    if line=="----":
        varLists.append([])
        continue
    varLists[len(varLists)-1].append(line)

print("--Differences:--")
diff = {}

l1, l2 = varLists
for i in l1+l2:
    if i in l1 and i in l2: pass
    elif i not in l1: diff.update({i: "modded"})
    elif i not in l2: diff.update({i: "original"})


print("number of differences:", len(diff))

for i,val in diff.items():
    print(f"{i:<21} |", end=' ')
    print(val, end='')
    print()
