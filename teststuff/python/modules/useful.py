#!/usr/bin/env python3

def sortDict(dict, mode=0):
    """
    return dict parameter sorted by mode
    
    modes:
    - 0. normal sort order
    """
    sortedDict = {}
    if mode==0:
        dictKeys = list(dict.keys())
        dictKeys.sort()
        sortedDict = {i: dict[i] for i in dictKeys}

    return sortedDict

def sortList_simple(lst, idx=0):
    """
    return list-of-lists parameter "lst" sorted by idx
    """
    tempDict = {}
    print(len(lst))
    for i in range(len(lst)):
        tempDict.update({lst[i][idx]:lst[i]})
    dictKeys = list(tempDict.keys())
    print("len",len(dictKeys))
    dictKeys.sort()
    print("len",len(dictKeys))
    sortedDict = {i: tempDict[i] for i in dictKeys}
    return [val for _,val in sortedDict.items()]


def findElIdx(nail, lst, skipIdx=[]):
    finds = [None]
    for i in range(len(lst)):
        if nail==lst[i] and i not in skipIdx:
            if finds[0]==None: finds=[]
            finds.append(i)
    return finds

def findRepet_lst(lst, mode=0):
    """
    Find and return repetations in parameter "lst"

    mode:
    - 0: return only lists of indexes to all elements with repeations
    - 1: return only lists of all matches
    - 2: return what was matches and a lists of lists of indexes to all elements
    - 3: return number of repetitions
    """
    matches = []
    toIgnore = []
    indexes = []
    base = lst[0]
    repet = 0
    for n in range(1, len(lst)):
        for i in range(0, len(lst)):
            if base==lst[i] and lst[i] not in toIgnore:
                foundIdx = findElIdx(lst[i],lst)
                if foundIdx[0]!=None:
                    repet+=len(foundIdx)
                    indexes.append(foundIdx)
                    toIgnore.append(lst[i])
                    matches.append([lst[n] for n in foundIdx])
        base=lst[n]
    if mode==0: return indexes
    elif mode==1: return matches
    elif mode==2: return [[matches[i][0],indexes[i]] for i in range(len(matches))]
    elif mode==3: return [repet]

if __name__=="__main__":
    from random import randrange

    test=0

    if test==0:
        testLen = 10
        temp = [[randrange(2),randrange(2)] for _ in range(testLen)]

        tempTemp = findRepet_lst(temp, 3)
        for i in tempTemp: print(i)
        print("",end="\n----------\n")

        sortedTemp = sortList_simple(temp, 1)

        for i in range(len(temp)):
            print(temp[i])
        print("")
        for i in range(len(sortedTemp)):
            print(sortedTemp[i])
        print("\nlen:", len(temp), len(sortedTemp))
    if test==1:
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