#!/usr/bin/env python3

#######################################################################################
# Progam: Inverse Kinematics for a 6DOF Robotic Arm Using an Analytical Approach
# Description: Given a desired end position (x, y, z) of the end effector of a robot, 
#   and a desired orientation of the end effector (relative to the base frame),
#   calculate the joint angles (i.e. angles for the servo motors).
# Author: Addison Sears-Collins
# Website: https://automaticaddison.com
# Date: October 19, 2020
# Reference: Sodemann, Dr. Angela 2020, RoboGrok, accessed 14 October 2020, <http://robogrok.com/>
#######################################################################################
 
import numpy as np # Scientific computing library 
 
# Define the desired position of the end effector
# This is the target (goal) location.
x = 4.0
y = 2.0
 
# Calculate the angle of the second joint
theta_2 = np.arctan2(y,x)
print(f'Theta 2 = {theta_2} radians\n')
 
# Define the desired orientation of the end effector relative to the base frame 
# (i.e. global frame)
# This is the target orientation.
# The 3x3 rotation matrix of frame 6 relative to frame 0
rot_mat_0_6 = np.array([[-1.0, 0.0, 0.0],
                        [0.0, -1.0, 0.0],
                        [0.0, 0.0, 1.0]])
 
# The 3x3 rotation matrix of frame 3 relative to frame 0
rot_mat_0_3 = np.array([[-np.sin(theta_2), 0.0, np.cos(theta_2)],
                        [np.cos(theta_2), 0.0, np.sin(theta_2)],
                        [0.0, 1.0, 0.0]])
 
# Calculate the inverse rotation matrix
inv_rot_mat_0_3 = np.linalg.inv(rot_mat_0_3)
 
# Calculate the 3x3 rotation matrix of frame 6 relative to frame 3
rot_mat_3_6 = inv_rot_mat_0_3 @ rot_mat_0_6
print(f'rot_mat_3_6 = {rot_mat_3_6}')
 
# We know the equation for rot_mat_3_6 from our pencil and paper
# analysis. The simplest term in that matrix is in the third column,
# third row. The value there in variable terms is cos(theta_5).
# From the printing above, we know the value there. Therefore, 
# cos(theta_5) = value in the third row, third column of rot_mat_3_6, which means...
# theta_5 = arccosine(value in the third row, third column of rot_mat_3_6)
theta_5 = np.arccos(rot_mat_3_6[2, 2])
print(f'\nTheta 5 = {theta_5} radians')
 
# Calculate the value for theta_6
# We'll use the expression in the third row, first column of rot_mat_3_6.
# -sin(theta_5)cos(theta_6) = rot_mat_3_6[2,0]
# Solving for theta_6...
# rot_mat_3_6[2,0]/-sin(theta_5) = cos(theta_6)
# arccosine(rot_mat_3_6[2,0]/-sin(theta_5)) = theta_6
theta_6 = np.arccos(rot_mat_3_6[2, 0] / -np.sin(theta_5))
print(f'\nTheta 6 = {theta_6} radians')
 
# Calculate the value for theta_4 using one of the other
# cells in rot_mat_3_6. We'll use the second row, third column.
# cos(theta_4)sin(theta_5) = rot_mat_3_6[1,2]
# cos(theta_4) = rot_mat_3_6[1,2] / sin(theta_5)
# theta_4 = arccosine(rot_mat_3_6[1,2] / sin(theta_5))
theta_4 = np.arccos(rot_mat_3_6[1,2] / np.sin(theta_5))
print(f'\nTheta 4 = {theta_4} radians')
 
# Check that the angles we calculated result in a valid rotation matrix
r11 = -np.sin(theta_4) * np.cos(theta_5) * np.cos(theta_6) - np.cos(theta_4) * np.sin(theta_6)
r12 = np.sin(theta_4) * np.cos(theta_5) * np.sin(theta_6) - np.cos(theta_4) * np.cos(theta_6)
r13 = -np.sin(theta_4) * np.sin(theta_5)
r21 = np.cos(theta_4) * np.cos(theta_5) * np.cos(theta_6) - np.sin(theta_4) * np.sin(theta_6)
r22 = -np.cos(theta_4) * np.cos(theta_5) * np.sin(theta_6) - np.sin(theta_4) * np.cos(theta_6)
r23 = np.cos(theta_4) * np.sin(theta_5)
r31 = -np.sin(theta_5) * np.cos(theta_6)
r32 = np.sin(theta_5) * np.sin(theta_6)
r33 = np.cos(theta_5)
 
check_rot_mat_3_6 = np.array([[r11, r12, r13],
                              [r21, r22, r23],
                              [r31, r32, r33]])
 
# Original matrix
print(f'\nrot_mat_3_6 = {rot_mat_3_6}')
 
# Check Matrix
print(f'\ncheck_rot_mat_3_6 = {check_rot_mat_3_6}\n')
 
# Return if Original Matrix == Check Matrix
rot_minus_check_3_6 = rot_mat_3_6.round() - check_rot_mat_3_6.round()
zero_matrix = np.array([[0, 0, 0],
                        [0, 0, 0],
                        [0, 0, 0]])
matrices_are_equal = np.array_equal(rot_minus_check_3_6, zero_matrix)
 
# Determine if the solution is valid or not
# If the solution is valid, that means the end effector of the robotic 
# arm can reach that target location.
if (matrices_are_equal):
  valid_matrix = "Yes"
else:
  valid_matrix = "No" 
print(f'Is this solution valid?\n{valid_matrix}\n')