import math
import time
import matplotlib.pyplot as plt
from random import randrange

servo = {}
for i in range(6): servo.update({i: 0})

line = {} #temporary for plt
for i in range(6): line.update({"q"+str(i+1):[[0],[0]]}) #temporary for plt


#unit: degrees/second
def sCustom_func(s, new_rotation, total_iteration, total_speed):
    global line #temporary for plt
    '''
    Args:
        s (float/int): dictionary/list variable that sends to pca board / holds old/current rotation commands
        new_rotation (float/int): dictionary/list variable of new rotation commands
        total_iteration (int): number of loop iterations to complete the movement
        total_speed (float/int): Total time spent moving the servo from start to finish in seconds
    '''
    counter = 0
    s_diff = {}
    for i in range(6):
        s_diff[i] = new_rotation[i]-s[i]
    while s[0]<new_rotation[0] or s[1]<new_rotation[1] or s[2]<new_rotation[2] or s[3]<new_rotation[3] or s[4]<new_rotation[4] or s[5]<new_rotation[5]: #dont need abs() because it goes from 0-180
        for i in range(6):
            s[i] += s_diff[i]/total_iteration
            line["q"+str(i+1)][0].append(counter) #temporary for plt
            line["q"+str(i+1)][1].append(s[i]) #temporary for plt
        print(s)
        counter+=+total_speed/total_iteration #temporary for plt
        time.sleep(total_speed/total_iteration)


servo = {0:90, 1:0, 2:0, 3:90, 4:0, 5:90}
test = {} #temporary for plt
for i in range(6): test.update({i:randrange(180)}) #temporary for plt

sCustom_func(test,[180]*6, 1000, 0.2)

for i in range(6): #temporary for plt
    plt.plot(line["q"+str(i+1)][0], line["q"+str(i+1)][1], label="q"+str(i+1)) #temporary for plt

plt.xlabel('x axis: [seconds]') #temporary for plt
plt.ylabel('y axis: [degrees]') #temporary for plt
plt.title('degree to rotation graph') #temporary for plt

plt.legend() #temporary for plt
plt.show() #temporary for plt