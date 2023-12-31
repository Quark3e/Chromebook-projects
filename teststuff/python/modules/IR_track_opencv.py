
import cv2
import math

def nothing(x):
    pass

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
    IR_HSV_Val = [[0, 0, 255], [179, 9, 255]]

    kernel = np.ones((5, 5), np.uint8)
    font = cv2.FONT_HERSHEY_SIMPLEX

    imgTemp = []
    morphImg = []
    thershImg = []

    ret = []
    contours = []
    cntArea = []
    cntMoments = []
    frame = []
    tempPos = []
    cntPos = []
    def __init__(
            self,
            camIdx,
            prefRes=[(640, 480)]
        ):
        for i in camIdx:
            self.cam.update({i:{"winname": f"cam{int(i)}", "vidcapt":cv2.VideoCapture(i)}})
            if toDisplay:
                cv2.namedWindow(self.cam[i]["winname"])
                hsv_trackbars(self.cam[i]["winname"], self.IR_HSV_Val)
            self.imgTemp.append(0)
            self.morphImg.append(0)
            self.threshImg.append(0)
            self.ret.append(0)
            self.contours.append(0)
            self.cntArea.append(0)
            self.cntMoments.append(0)
            self.frame.append(0)
            self.tempPos.append(0)
    def processFrame(self, img, flag, winName):
        hsvList = self.IR_HSV_Val

        self.frame[flag] = cv2.resize(img, prefRes)
        self.frame[flag] = cv2.flip(self.frame[flag], 1)
        hsv = cv2.cvtColor(self.frame[flag], cv2.COLOR_BGR2HSV)

        if displayToOpenCV:
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

        _, self.thresh[flag] = cv2.threshold(self.morphImg[flag], 127, 255, cv2.THRESH_BINARY)
        self.contours[flag], hierarchy = cv2.findContours(cv2.cvtColor(thresh[flag], cv2.COLOR_BGR2GRAY), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)




