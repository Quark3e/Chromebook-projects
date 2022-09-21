#!/usr/bin/env python3

#Transfers acc x and y values as column 3 and 4 with default values on column 1 and 2

import readline


rowLine = 36

basicFileName = "basicTest_1"
readFileName = basicFileName + "_read"
resultFileName = basicFileName + "_result"


directPath = "/home/berkhme/vscode repo/SchoolRelated/teststuff/python/random/data files/testResult/"
fileExtension = ".dat"

basicFile = open(directPath + basicFileName + fileExtension, "r")
readFile = open(directPath + readFileName + fileExtension, "r")
resultFile = open(directPath + resultFileName + fileExtension, "w")


for i in range(36):
    basicLine = basicFile.readline()
    readLine = readFile.readline()
    resultLine = basicLine[0:basicLine.find(" ") + 1] #It includes the space
    basicLine = basicLine.replace(basicLine[0:basicLine.find(" ") + 1], '', 1)
    resultLine = resultLine + basicLine[0:basicLine.find(" ") + 1] #includes  space
    resultLine = resultLine + readLine[0:readLine.find(" ") + 1] #includes space
    readLine = readLine.replace(readLine[0:readLine.find(" ") + 1], '', 1)
    resultLine = resultLine + readLine[0:readLine.find(" ") + 1] #includes space
    
    resultFile.write(resultLine + "\n")

basicFile.close()
readFile.close()
resultFile.close()