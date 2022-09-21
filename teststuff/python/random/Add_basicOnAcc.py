#!/usr/bin/env python3

#Transfers acc x and y values as column 3 and 4 with default values on column 1 and 2

rowLine = 36

basicFileName = "basicTest_1"
accFileName = basicFileName + "_acc"
resultFileName = basicFileName + "_result"


directPath = "/home/berkhme/vscode repo/SchoolRelated/teststuff/python/random/data files/testResult/"
fileExtension = ".dat"

basicFile = open(directPath + basicFileName + fileExtension, "r")
accFile = open(directPath + accFileName + fileExtension, "r")
resultFile = open(directPath + resultFileName + fileExtension, "w")


for i in range(36):
    basicLine = basicFile.readline()
    accLine = accFile.readline()
    resultLine = basicLine[0:basicLine.find(" ") + 1] #It includes the space
    basicLine = basicLine.replace(basicLine[0:basicLine.find(" ") + 1], '', 1)
    resultLine = resultLine + basicLine[0:basicLine.find(" ") + 1] #includes  space
    resultLine = resultLine + accLine[0:accLine.find(" ") + 1] #includes space
    accLine = accLine.replace(accLine[0:accLine.find(" ") + 1], '', 1)
    resultLine = resultLine + accLine[0:accLine.find(" ") + 1] #includes space
    
    resultFile.write(resultLine + "\n")

basicFile.close()
accFile.close()
resultFile.close()