
import cv2 #type: ignore




def nothing(x):
    pass

def hsv_trackbars(winName, presetVal):
    cv2.createTrackbar("L - H", winName, presetVal[0][0], 179, nothing)
    cv2.createTrackbar("L - S", winName, presetVal[0][1], 255, nothing)
    cv2.createTrackbar("L - V", winName, presetVal[0][2], 255, nothing)
    cv2.createTrackbar("U - H", winName, presetVal[1][0], 179, nothing)
    cv2.createTrackbar("U - S", winName, presetVal[1][1], 255, nothing)
    cv2.createTrackbar("U - V", winName, presetVal[1][2], 255, nothing)
    

