#!/usr/bin/env python3

# Import essential libraries
from cv2 import FILLED
from pyparsing import White
import requests
import cv2 #type: ignore
import numpy as np
import imutils
import mediapipe as mp
import math
import time

mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_hands = mp.solutions.hands



# Replace the below URL with your own. Make sure to add "/shot.jpg" at last.
useDefaultAddress = True
url_start = "http://"
url_end = "/shot.jpg"

if useDefaultAddress:
    url = url_start + "192.168.1.124:8080" + url_end
else:
    url = url_start + input('Enter full ip address including port: ') + url_end
  
imgRes = [float(640), float(480)]
zDefault = 300
xScaling, yScaling = 0.5, 0.5

# For static images:
IMAGE_FILES = []
with mp_hands.Hands(
    static_image_mode=False,
    max_num_hands=2,
    min_detection_confidence=0.5) as hands:
  for idx, file in enumerate(IMAGE_FILES):
    # Read an image, flip it around y-axis for correct handedness output (see
    # above).
    image = cv2.flip(cv2.imread(file), 1)
    # Convert the BGR image to RGB before processing.
    results = hands.process(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))

    # Print handedness and draw hand landmarks on the image.
    print('Handedness:', results.multi_handedness)
    if not results.multi_hand_landmarks:
      continue
    image_height, image_width, _ = image.shape
    annotated_image = image.copy()
    for hand_landmarks in results.multi_hand_landmarks:
      print('hand_landmarks:', hand_landmarks)
      print(
          f'Index finger tip coordinates: (',
          f'{hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP].x * image_width}, '
          f'{hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP].y * image_height})'
      )
      mp_drawing.draw_landmarks(
          annotated_image,
          hand_landmarks,
          mp_hands.HAND_CONNECTIONS,
          mp_drawing_styles.get_default_hand_landmarks_style(),
          mp_drawing_styles.get_default_hand_connections_style())
    cv2.imwrite(
        '/tmp/annotated_image' + str(idx) + '.png', cv2.flip(annotated_image, 1))
    # Draw hand world landmarks.
    if not results.multi_hand_world_landmarks:
      continue
    for hand_world_landmarks in results.multi_hand_world_landmarks:
      mp_drawing.plot_landmarks(
        hand_world_landmarks, mp_hands.HAND_CONNECTIONS, azimuth=5)

with mp_hands.Hands(
    model_complexity=0,
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5) as hands:
    
    handPoints = []
    start_time = time.time()
    while True:
        img_resp = requests.get(url)
        img_arr = np.array(bytearray(img_resp.content), dtype=np.uint8)
        image = cv2.imdecode(img_arr, -1)
        image = cv2.flip(image, 1)
        # To improve performance, optionally mark the image as not writeable to
        # pass by reference.
        image.flags.writeable = False
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        results = hands.process(image)

        # Draw the hand annotations on the image.
        image.flags.writeable = True
        image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
        if results.multi_hand_landmarks:
            for hand_landmarks in results.multi_hand_landmarks:
                pointPos = [xScaling*(hand_landmarks.landmark[9].x*imgRes[0]-imgRes[0]*0.5), yScaling*(imgRes[1]-imgRes[1]*hand_landmarks.landmark[9].y),
                zDefault-math.sqrt(pow(abs(hand_landmarks.landmark[0].y*imgRes[1]-hand_landmarks.landmark[5].y*imgRes[1]), 2)+pow(abs(hand_landmarks.landmark[0].x*imgRes[0]-hand_landmarks.landmark[5].x*imgRes[0]), 2))
                ]
                print(" x:", pointPos[0], " y:", pointPos[1], " z:", pointPos[2], sep='')
                # cv2.circle(image, (int(pointPos[0]/xScaling+imgRes[0]*0.5), int(imgRes[1]/yScaling-pointPos[1])), 10, (0, 0, 0), -1)
                cv2.circle(image, (int(hand_landmarks.landmark[9].x*image.shape[1]), int(hand_landmarks.landmark[9].y*image.shape[0])), 10, (0, 0, 0), -1)

                # mp_drawing.draw_landmarks(
                #     image,
                #     hand_landmarks,
                #     mp_hands.HAND_CONNECTIONS,
                #     mp_drawing_styles.get_default_hand_landmarks_style(),
                #     mp_drawing_styles.get_default_hand_connections_style())
        fps = int(round(1 / (time.time()-start_time)))
        print("fps:", fps, end='')
        # print()
        cv2.imshow('MediaPipe Hands', cv2.resize(image, None, fx=1, fy=1))
        if cv2.waitKey(5) & 0xFF == 27: # or cv2.waitKey(5) == 27:
            break
        start_time = time.time()
cv2.destroyAllWindows()


