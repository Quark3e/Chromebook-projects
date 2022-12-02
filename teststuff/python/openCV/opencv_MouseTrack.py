#!/usr/bin/env python3
# Import packages
import cv2
 
# Lists to store the bounding box coordinates
top_left_corner=[]
bottom_right_corner=[]
temp_bottom_right_corner=[]
buttonPressed = False


l = 0
def printTest():
  global l
  l = l+1
  print("test:", l)


# function which will be called on mouse input
def drawRectangle(action, x, y, flags, *userdata):
  # Referencing global variables 
  global top_left_corner, bottom_right_corner, temp_bottom_right_corner, buttonPressed
  # Mark the top left corner when left mouse button is pressed
  if action == cv2.EVENT_LBUTTONDOWN:
    top_left_corner = [(x,y)]
    buttonPressed = True
    # When left mouse button is released, mark bottom right corner
  elif action == cv2.EVENT_LBUTTONUP:
    bottom_right_corner = [(x,y)]    
    # Draw the rectangle
    cv2.rectangle(image, top_left_corner[0], bottom_right_corner[0], (0,255,0),2, 8)
    cv2.imshow("Window",image)
    buttonPressed = False
  elif buttonPressed:
    temp_bottom_right_corner = [(x,y)]    
    # Draw the rectangle
    cv2.rectangle(image, top_left_corner[0], temp_bottom_right_corner[0], (0,255,0),2, 8)
    cv2.imshow("Window",image)

 
# Read Images
image = cv2.imread("/home/berkhme/Chromebook-projects/teststuff/python/openCV/img1.jpg")
# Make a temporary image, will be useful to clear the drawing
image = cv2.resize(image, (640, 360))
temp = image
# Create a named window
cv2.namedWindow("Window")
# highgui function called when mouse events occur
cv2.setMouseCallback("Window", drawRectangle)
 
k=0
# Close the window when key q is pressed
while k!=113:
  image = temp
  # Display the image
  cv2.imshow("Window", image)
  k = cv2.waitKey(0)
  # If c is pressed, clear the window, using the dummy image
  if k == 99:
    image= temp
    cv2.imshow("Window", image)
 
cv2.destroyAllWindows()