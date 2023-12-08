#!/usr/bin/env python3

def sortDict(dict, mode=0):
    """
    Sort functions
    """
    sortedDict = {}
    if mode==0:
        dictKeys = list(dict.keys())
        dictKeys.sort()
        sortedDict = {i: dict[i] for i in dictKeys}

    return sortedDict




if __name__=="__main__":
    from random import randrange
    fileLen = 50
    temp = {randrange(100):randrange(100) for _ in range(fileLen)}
    # print(temp, end="\n\n")
    for key,val in temp.items():
        print(key, val)
    print()
    print(len(temp), end="\n\n")
    sorttemp = sortDict(temp, 0)
    # print(sorttemp)

    fullList = [[[],[]] for _ in range(len(temp))]
    i=0
    for key,val in temp.items():
        fullList[i][0] = [key, val]
        i+=1
    i=0
    for key,val in sorttemp.items():
        fullList[i][1] = [key, val]
        i+=1

    for el in fullList:
        print(el)