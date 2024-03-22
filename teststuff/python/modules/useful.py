#!/usr/bin/env python3

import time
import math
from datetime import datetime, timedelta
import sys
import os
from copy import deepcopy


def getLongestStrInList(lst: list):
    """Find the string in list with the most number of characters

    ### Args:
        lst (list: str): list to check

    ### Returns:
        str: the longest string
    """
    # printing original list 
    # print("The original list : " + str(test_list))
    
    # Longest String in list
    # using loop
    max_len = -1
    for ele in lst:
        if len(ele) > max_len:
            max_len = len(ele)
            res = ele
    
    # printing result
    # print("Maximum length string is : " + res)
    return res

class basic_perf(object):

    delay_unit  = "seconds"
    delay_prefix= "sec"
    
    fps_unit    = "frames per second"
    fps_prefix  = "fps"

    templateDict = {
        "tA":   0,
        "tB":   0,
        "intervCount": 0,
        "delay": {
            "value": 0,
            "unit": delay_unit,
            "prefix": delay_prefix
        },
        "fps": {
            "value": 0,
            "unit": fps_unit,
            "prefix": fps_prefix
        }
    }

    perfDict = {}
    """Full dictionary that holds every variable for each name as base-key
    """
    baseInterVal = 1

    longest_name_len = 0
    print_precision_delay = 2
    print_precision_fps = 1
    print_unit_delay_ms = True
    print_delay_padding_default = 6

    def __init__(
            self,
            nameList: list,
            interval = 1,
            ):
        self.baseInterVal = interval
        self.longest_name_len = len(getLongestStrInList(nameList))

        for name in nameList:
            self.perfDict.update( {name: deepcopy(self.templateDict)} )

        self.perfDict["get_camPos"]["delay"]["value"]=69

        # for key,value in self.perfDict.items():
        #     print(f" -{key:<12}:", value)

        # input("paused..")

    def tA(self, name):
        if name not in self.perfDict:
            print(f"\nERROR: basic_perf: tA() {name} not in dict")
            return
        if self.perfDict[name]["intervCount"]==0:
            self.perfDict[name]["tA"] = time.perf_counter()
    def tB(self, name):
        if name not in self.perfDict:
            print(f"\nERROR: basic_perf: tB() {name} not in dict")
            return
        self.perfDict[name]["tB"] = time.perf_counter()
        if self.perfDict[name]["intervCount"]>=self.baseInterVal-1:
            self.perfDict[name]["delay"]["value"] = (self.perfDict[name]["tB"] - self.perfDict[name]["tA"])/self.baseInterVal
            self.perfDict[name]["fps"]["value"] = 1/self.perfDict[name]["delay"]["value"]
            self.perfDict[name]["intervCount"] = 0
            # print(self.perfDict[name]["delay"]["value"])
        else:
            self.perfDict[name]["intervCount"] +=1
    def print(self,
              ignoreStr=[]
              ):
        toPrintString = ""
        sumDelay = 0
        winDim = [1+2+self.longest_name_len+2+7+2+1, 1+len(self.perfDict)+1]
        
        toPrintString += winDim[0]*"-"+"\n"
        toPrintString += "|{temp:<{pad}}|\n".format(temp="Performance:",pad=winDim[0]-1)
        for key,value in self.perfDict.items():
            temp = ""
            temp += "| -{nomKey:<{pad}}: ".format(nomKey=key, pad=self.longest_name_len)
            if self.print_unit_delay_ms:
                temp += "{delay:>{pad}}ms".format(
                    delay=round(value["delay"]["value"]*1000,self.print_precision_delay),
                    pad=self.print_delay_padding_default
                )
            else:
                temp += "{delay:>{pad}}s".format(
                    delay=round(value["delay"]["value"],self.print_precision_delay),
                    pad=self.print_delay_padding_default-2
                )
            toPrintString += "{tempStr:<{pad}}|\n".format(tempStr=temp, pad=winDim[0])
            if not all([s in key for s in ignoreStr]):
                sumDelay += value["delay"]["value"]
        sumDelay+=0.01
        temp = f"|total: [{round(sumDelay*1000,2):>6}ms] [fps:{round(1/sumDelay,1):>6}]"
        toPrintString += "{tempStr:<{pad}}\n".format(tempStr=temp, pad=winDim[0])

        # sys.stdout.write(
        #     "\x1B[2J\x1B[H"+
        #     toPrintString
        # )
        return toPrintString


simpleANSIwrite_init = False
def simpleANSIWrite(
    text: str,
    textPos     = [0,0],
    doFlush     = True,
    clearScr    = False,
    textDelim   = "\n",
    endSymb     = "\n"
):
    global simpleANSIwrite_init
    """Write text/paragraph on terminal at `textPos`

    Args:
        - text (str): text to print on terminal
        - textPos (list, str): Where to print the text.
            If type==str then either `\"left\"` align or `\"right\"` align on the terminal.
            If type==list then place the text at that [x, y] coordinate
            Defaults to [0,0].
        - doFlush (bool): Whether to flush the terminal after printing text. Defaults to True.
        - clearScr (bool): Whether to clear the screen before every print. Defaults to False.
        - textDelim (str): Delimiter to separate full string by. Defaults to `"\\n"`.
        - endSymb (str): What to print after printing `text`. Defaults to "\\n".
    """
    ansiCode = "\x1B["
    if clearScr: sys.stdout.write(ansiCode+"2J")
    tempLst = text.split(textDelim)

    termSize = [os.get_terminal_size().columns, os.get_terminal_size().lines]
    printPos = [0, 0]
    maxRowLen = max([len(row) for row in tempLst])

    if not simpleANSIwrite_init:
        sys.stdout.write(ansiCode+"2J")
        simpleANSIwrite_init = True

    if type(textPos)==list:
        if type(textPos[0])==str:
            if textPos[0]=="left":
                printPos[0] = 0
            elif textPos[0]=="right":
                printPos[0] = termSize[0]-maxRowLen
        elif type(textPos[0])==int:
            printPos[0] = textPos[0]
        if type(textPos[1])==str:
            if textPos[1]=="top":
                printPos[1] = 0
            elif textPos[1]=="bottom":
                printPos[1] = termSize[1]-len(tempLst)
        elif type(textPos[1])==int:
            printPos[1] = textPos[1]
        for i in range(len(tempLst)):
            sys.stdout.write(
                ansiCode + f"{printPos[1]+i};{printPos[0]}H" + tempLst[i]
            )
    elif type(textPos)==str:
        if textPos=="left":
            for i in range(len(tempLst)):
                sys.stdout.write(
                    ansiCode + f"0;0H" + tempLst[i]
                )
        elif textPos=="right":
            for i in range(len(tempLst)):
                sys.stdout.write(
                    ansiCode + f"{i};{termSize[0]-maxRowLen}H" + tempLst[i]
                )
    sys.stdout.write(endSymb)
    if doFlush: sys.stdout.flush()
def simpleANSIWrite2(
    text: list,
    textPos     = [0, 0],
    doFlush     = True,
    clearScr    = False,
    endSymb     = "\n"
):
    ansiCode = "\x1B["
    if clearScr: sys.stdout.write(ansiCode+"2J")
    for i in range(len(text)):
        sys.stdout.write(
            ansiCode + f"{textPos[1]+i};{textPos[0]}H" + text[i]
        )
    sys.stdout.write(endSymb)
    if doFlush: sys.stdout.flush()

progbar_progress = 1
progbar_total = 1

def replaceSubstr(
        fullStr,
        newStr,
        start_index,
        upTo_index
    ):
    return fullStr[:start_index] + newStr + fullStr[upTo_index:]

def printProgBar(symbIdx=4):
    """
    prints a progress bar

    Parameters:
    - total: total number for progress to be at 100%
    - progress: current number for progress (progress = [current/total)*100]
    - symbIdx: indexing integer for symbol to use: ['■', '⬛', '▉', '▉', '█']
    """
    oldProg=0
    percent=0
    speed=0
    count=0
    # checkCount=[0, 3, 0.08]
    img = {
        0: "|",
        1: "/",
        2: "-",
        3: "\\",
    }
    symb = ['■', '⬛', '▉', '▉', '█']
    startDate = datetime.now()
    print(f" {'Start time':<10}:", startDate)
    progBar_t0 = time.perf_counter()
    time.sleep(0.1)
    while progbar_progress<=progbar_total:
        t1 = time.perf_counter()
        speed = (progbar_progress-oldProg)/(t1-progBar_t0)
        if speed==0: speed=1
        print(speed)

        percent = (progbar_progress/progbar_total)*100
        formatProgress = f"{progbar_progress:_}"
        if count>=len(img): count=0
        printStr = f" progress: {formatProgress:>10}: {round(percent,2):<5}% \
              |{str(math.floor(percent)*symb[symbIdx]+img[count]):<100}|: \
              {round(speed,1):<6}pt/s: ETA: {timedelta(seconds=(round((progbar_total-oldProg)/speed)))} \
              "
        print(printStr, end="\r")
        count+=1
        oldProg=progbar_progress
        progBar_t0=time.perf_counter()
        yield printStr


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
    # tempList = [] #[val, index, fullline]
    # for i in range(len(lst)):
    #     print("update")
    #     tempList.append([lst[i][idx], i, lst[i]])
    # lstKeys = [tempList[i][0] for i in range(len(tempList))]
    # lstKeys.sort()
    # sortedList = [lst[lstKeys[1]] for i in range(len(lstKeys))]
    sorted = lst.copy()
    sorted.sort()
    return sorted


def findElIdx(nail, lst, skipIdx=[]):
    finds = [0]
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
        testLen = 100
        rrange = [2, 5, 10]
        
        temp = [[randrange(rrange[0]),randrange(rrange[1]),randrange(rrange[2])] for _ in range(testLen)]

        tempTemp = findRepet_lst(temp, 3)
        for i in tempTemp: print(i)
        print("",end="\n----------\n")

        sortedTemp = sortList_simple(temp, 0)

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