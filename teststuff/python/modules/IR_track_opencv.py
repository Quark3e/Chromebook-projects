
import cv2
import math
import numpy as np

def nothing(x):
    pass

def solveContours(allContours, areaThreshold):
    allPositions = [[], []]
    totArea = 0
    if len(allContours)==0: return [[0,0], 0]
    # print(" len():", len(allContours), end=" ")
    for cnt in allContours:
        CntMoments = cv2.moments(cnt)
        if CntMoments["m00"] != 0:
            area = cv2.contourArea(cnt)
            # print(area, end=" ")
            if area>=areaThreshold:
                allPositions[0].append(CntMoments["m10"]/CntMoments["m00"])
                allPositions[1].append(CntMoments["m01"]/CntMoments["m00"])
                totArea+=area
    avgPos = [
        int(round(sum(allPositions[0])/len(allPositions[0]))),
        int(round(sum(allPositions[1])/len(allPositions[1])))
    ]
    return [avgPos, int(totArea)]

def hsv_trackbars(winName, presetVal):
    cv2.createTrackbar("L - H", winName, presetVal[0][0], 179, nothing)
    cv2.createTrackbar("L - S", winName, presetVal[0][1], 255, nothing)
    cv2.createTrackbar("L - V", winName, presetVal[0][2], 255, nothing)
    cv2.createTrackbar("U - H", winName, presetVal[1][0], 179, nothing)
    cv2.createTrackbar("U - S", winName, presetVal[1][1], 255, nothing)
    cv2.createTrackbar("U - V", winName, presetVal[1][2], 255, nothing)
    


class IR_camTrack(object):
    toDisplay = True
    cam = {}
    """Dictionary to hold videoCapture object, windowname
        {
            idx: {
                "winname": string windowname,
                "vidcapt": cv2.VideoCapture object,
            }
        }
    """
    imgWinScal = [0.4, 0.4]

    IR_HSV_Val = [[0, 0, 255], [179, 9, 255]]

    kernel = np.ones((5, 5), np.uint8)
    font = cv2.FONT_HERSHEY_SIMPLEX

    imgTemp = {int: cv2.Mat}
    morphImg = {int: cv2.Mat}
    threshImg = {int: cv2.Mat}

    ret = {int: bool}
    contours = {}
    cntArea = {}
    cntMoments = {}
    frame = {int: cv2.Mat}
    tempPos = {2:[0, 0], 0:[0, 0]}
    def __init__(self):
        """Empty class initialization. To use class methods, self.setup(...) function must be called. 
        \n Purpose of empty __init__ func is to be able to use class object as variable holder
        """
        pass
    def setup(
            self,
            camIdx: list,
            prefres=[(int(640), int(480))],
            displayWindows=True
        ):
        """Class setup

        Args:
            camIdx (list, int): List of indexes to connected webcam.
            prefres (list, int): prefered resolution for cv tracking. Lower means faster but less detailed. Defaults to [(int(640), int(480))].
            displayWindows (bool): Whether to display to a separate window. Defaults to True.
        """
        self.toDisplay = displayWindows
        if len(camIdx) != len(prefres):
            self.prefRes = {}
            for i in camIdx: self.prefRes.update({i:prefres[0]})
        self.tempPos = {}
        for i in camIdx:
            self.cam.update({i:{"winname": f"cam{int(i)}", "vidcapt":cv2.VideoCapture(i)}})
            self.cam[i]["vidcapt"].set(cv2.CAP_PROP_AUTO_EXPOSURE, 1) # Set exposure to manual mode
            
            if self.toDisplay:
                cv2.namedWindow(self.cam[i]["winname"])
                hsv_trackbars(self.cam[i]["winname"], self.IR_HSV_Val)
                cv2.moveWindow(self.cam[i]["winname"], round(i/2*self.prefRes[0][0]*self.imgWinScal[0]), 10)
            self.imgTemp.update({i:0})
            self.morphImg.update({i:0})
            self.threshImg.update({i:0})
            self.ret.update({i:0})
            self.contours.update({i:0})
            self.cntArea.update({i:0})
            self.cntMoments.update({i:0})
            self.frame.update({i:0})
            self.tempPos.update({i:0})
    def testCamWin(self):
        self.update()
    def update(self):
        """complete update of class
            - read new images from self.cam
            - process captured images
            - (if self.toDisplay==True) display the images to window

        Args: nothing
        Return: nothin
        """
        for i in self.cam:
            self.ret[i], self.imgTemp[i] = self.cam[i]["vidcapt"].read()
        if False in [val for _,val in self.ret.items()]:
            print(f"Error: Could not open camera: cam idx{self.ret}")
            return None
        for i in self.cam:
            self.processFrame(self.imgTemp[i], i)
            self.tempPos[i], self.cntArea[i] = solveContours(self.contours[i], 0)

            if self.toDisplay:
                cv2.imshow(self.cam[i]["winname"], cv2.resize(np.vstack((self.morphImg[i],self.frame[i])), None, fx=self.imgWinScal[0], fy=self.imgWinScal[1]))
                key = cv2.waitKey(5)
                if key==27: return None
        return 0
    def processFrame(
            self,
            img: cv2.Mat,
            flag: int,
        ):
        """Process img with preset method for getting contour mask and tracked contours

        Args:
            img (np.ndarray, cv2.Mat): cv2.Mat image to process
            flag (int): what frame to process out of the available
            winName (str): window name [NOTE: not used]
        Return:
            nothing.
        """
        hsvList = self.IR_HSV_Val

        self.frame[flag] = cv2.resize(img, self.prefRes[flag])
        self.frame[flag] = cv2.flip(self.frame[flag], 1)
        hsv = cv2.cvtColor(self.frame[flag], cv2.COLOR_BGR2HSV)

        if self.toDisplay:
            hsvList[0][0] = cv2.getTrackbarPos("L - H", self.cam[flag]["winname"])
            hsvList[0][1] = cv2.getTrackbarPos("L - S", self.cam[flag]["winname"])
            hsvList[0][2] = cv2.getTrackbarPos("L - V", self.cam[flag]["winname"])
            hsvList[1][0] = cv2.getTrackbarPos("U - H", self.cam[flag]["winname"])
            hsvList[1][1] = cv2.getTrackbarPos("U - S", self.cam[flag]["winname"])
            hsvList[1][2] = cv2.getTrackbarPos("U - V", self.cam[flag]["winname"])
            mask = cv2.inRange(hsv, np.array(hsvList[0]), np.array(hsvList[1]))
        else:
            mask = cv2.inRange(hsv, np.array(hsvList[0]), np.array(hsvList[1]))
        propMask = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)


        self.morphImg[flag] = cv2.erode(cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR), self.kernel, iterations=1)
        self.morphImg[flag] = cv2.dilate(self.morphImg[flag], self.kernel, iterations=6)

        _, self.threshImg[flag] = cv2.threshold(self.morphImg[flag], 127, 255, cv2.THRESH_BINARY)
        self.contours[flag], hierarchy = cv2.findContours(cv2.cvtColor(self.threshImg[flag], cv2.COLOR_BGR2GRAY), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    def close(
            self
        ):
        """Releases all videocapture objects."""
        print("releasing video captures")
        for key,val in self.cam.items():
            self.cam[key]["vidcapt"].release()



