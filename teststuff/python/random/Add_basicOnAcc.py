#!/usr/bin/env python3

#Transfers acc x and y values as column 3 and 4 with default values on column 1 and 2

import readline
import sys

rowLine = 68
folderName = "test2"
angleName = "_-15.-45"
basicFileName = "basicTest_2"
readFileName = basicFileName + angleName  + "_read"
resultFileName = basicFileName + angleName + "_result"


directPath = "/home/berkhme/vscode repo/Chromebook-projects/teststuff/python/random/data files/testResult/" + folderName + "/"
fileExtension = ".dat"

basicFile = open(directPath + basicFileName + fileExtension, "r")
readFile = open(directPath + readFileName + fileExtension, "r")
resultFile = open(directPath + resultFileName + fileExtension, "r+")

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


if __name__ == "__main__":
    while True:
        print(" Options:")
        print("1. Add \"Read\"-position values beside \"Given\"-position values")
        print("2. Get average/mean error value of given column")
        print("3. exit")

        option = int(input("input: "))
        if option==1:
            addReadOnBasic()
        if option==2:
            getMeanValue()
        elif option==3:
            basicFile.close()
            readFile.close()
            resultFile.close()
            sys.exit()



basicFile.close()
readFile.close()
resultFile.close()


