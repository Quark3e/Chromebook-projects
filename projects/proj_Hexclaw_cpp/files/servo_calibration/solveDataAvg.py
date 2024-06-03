#!/usr/bin/env python3

from math import * #type: ignore


data = []
variations = 0
solveAvg_char = "4"


def loadData():
    global data, variations
    delayFile = open("readDelays.dat", "r")
    # line = delayFile.readline()
    for line in delayFile:
        if line[:1]==solveAvg_char:
            # print(line)
            data.append([])
            line = line.replace(";", "")
            for var in line[2:].split(","):
                data[len(data)-1].append(float(var))
            variations+=1
    delayFile.close()


def main():
    sumList = 19*[0]

    for l in range(19):
        for lst in range(len(data)-1):
            sumList[l] += data[lst][l]
        sumList[l] = sumList[l]/len(data)
    print("number of sum addends:", variations, sep='')
    print(len(sumList), sumList)

    return


if __name__=="__main__":
    loadData()
    main()