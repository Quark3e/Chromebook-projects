#!/usr/bin/env python3

#Transfers acc x and y values as column 3 and 4 with default values on column 1 and 2

import readline


rowLine = 68
folderName = "test2"
angleName = "_0.-45"
basicFileName = "basicTest_2"
readFileName = basicFileName + angleName  + "_read"
resultFileName = basicFileName + angleName + "_result"


directPath = "/home/berkhme/vscode repo/SchoolRelated/teststuff/python/random/data files/testResult/" + folderName + "/"
fileExtension = ".dat"

basicFile = open(directPath + basicFileName + fileExtension, "r")
readFile = open(directPath + readFileName + fileExtension, "r")
resultFile = open(directPath + resultFileName + fileExtension, "w")


for i in range(rowLine):
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