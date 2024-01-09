
#pragma once
#ifndef H_USEFUL
#define H_USEFUL

#include <math.h>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

/// @brief Convert radians to degrees
/// @param radians radians to convert. type: float()
/// @return return degrees. type: float()
float toDegrees(float radians) { return (radians*180)/M_PI; }
/// @brief Convert degrees to radians
/// @param degrees degrees to convert. type: float()
/// @return return radians. type: float()
float toRadians(float degrees) { return float(degrees*M_PI)/180; }


/// @brief Solve distance value between two coordinates in a 3D spacec
/// @param p1 float()[3]: point 1 coordinate {x, y, z}
/// @param p2 float()[3]: point 2 coordinate {x, y, z}
/// @return float() type of absolute straight distance
float get3dDistance(float p1[3], float p2[3]) { return sqrt(pow(p2[0]-p1[0],2) + pow(p2[1]-p1[1],2) + pow(p2[2]-p1[2],2)); }


/// @brief Solve whether a number is positive or negative (same as int(var/abs(var)))
/// @param var variable to check
/// @return 1 if var>0; -1 if var<0; 0 if var==0
int PoN(float var) {
	if(var>0) return 1;
	else if(var<0) return -1;
	else {
		cout << "PoN(): \"" << var << "\" is not a number\n";
		return 0;
	}
}


/// @brief Find index to closest value in arr
/// @param arr array to find closest index of.
/// @param needle value to find the closest to in the array
/// @param printVar whether to print results
/// @return index of result (closest value)
int getClosestValIdx(vector<float> arr, float needle, bool printVar=false) {
    vector<float> diffVec;
    vector<int> foundIdx;
    float minVar;

    for(int i=0; i<arr.size(); i++) { diffVec.push_back(abs(arr.at(i)-needle)); }
    minVar = *min_element(diffVec.begin(), diffVec.end());
    for(int i=0; i<diffVec.size(); i++) { if(diffVec.at(i)==minVar) foundIdx.push_back(i); }
    if(foundIdx.size()==0) return -1;
    return foundIdx.at(0);
}

#endif