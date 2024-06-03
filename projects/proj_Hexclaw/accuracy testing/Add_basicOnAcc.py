#!/usr/bin/env python3

#Transfers acc x and y values as column 3 and 4 with default values on column 1 and 2

import readline
import sys

rowLine = 68
folderName = "test2"
angleName = "_45.-45"
basicFileName = "basicTest_2"
readFileName = basicFileName + angleName  + "_read"
resultFileName = basicFileName + angleName + "_result"


directPath = "/home/berkhme/Chromebook-projects/projects/proj_4-FullRpi/accuracy testing/" + folderName + "/"
fileExtension = ".dat"

basicFile = open(directPath + basicFileName + fileExtension, "r")
readFile = open(directPath + readFileName + fileExtension, "r")
resultFile = open(directPath + resultFileName + fileExtension, "w")

def addReadOnBasic():
    global basicLine, readLine, resultLine, resultFile
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

def getMeanValue():
    totalVal = 0.0
    columnChoice = int(input("\nenter x or y column numb. to get value of (first column = index 0), x=0, y=1: "))
    for i in range(rowLine):
        tempLine = resultFile.readline()
        print("tempLine: \"", tempLine, "\"")
        totalVal+=abs(int(tempLine.split(" ")[columnChoice+2])-int(tempLine.split(" ")[columnChoice]))
        print("i:", i, " total:", totalVal)
    answer = totalVal / rowLine
    print("Mean value: ", answer)
    input("\nEnter any key and Enter to continue...")


addReadOnBasic()

basicFile.close()
readFile.close()
resultFile.close()


