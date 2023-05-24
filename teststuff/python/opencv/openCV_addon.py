
import cv2

def nothing(x):
    pass


def hsv_trackbars(winName):
    cv2.createTrackbar("L - H", winName, 0, 179, nothing)
    cv2.createTrackbar("L - S", winName, 0, 255, nothing)
    cv2.createTrackbar("L - V", winName, 255, 255, nothing)
    cv2.createTrackbar("U - H", winName, 179, 179, nothing)
    cv2.createTrackbar("U - S", winName, 9, 255, nothing)
    cv2.createTrackbar("U - V", winName, 255, 255, nothing)
    

