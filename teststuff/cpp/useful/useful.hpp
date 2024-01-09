
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
/// @param arr vector<float>: the hay in which to find the needle in
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
/// @brief Find index to closest value in arr
/// @param arr arr[]: the 
/// @param arrLen size of the array
/// @param needle value to find the closest to in the array
/// @param printVar whether to print results
/// @return index of result (closest value)
int getClosestValIdx(float arr[], int arrLen, float needle, bool printVar=false) {
    vector<float> diffVec;
    vector<int> foundIdx;
    float minVar;

    for(int i=0; i<arrLen; i++) { diffVec.push_back(abs(arr[i]-needle)); }
    minVar = *min_element(diffVec.begin(), diffVec.end());
    for(int i=0; i<diffVec.size(); i++) { if(diffVec.at(i)==minVar) foundIdx.push_back(i); }
    if(foundIdx.size()==0) return -1;
    return foundIdx.at(0);
}


void splitString(string line, string delimiter, float returnArr[], int numVar=4, bool printVar=false) {
    if(printVar) cout << "--- \"" << line << "\"\n";
    size_t pos = 0;
    for(int i=0; i<numVar; i++) {
        if(i<(numVar-1)) pos = line.find(delimiter);
        if(printVar) cout << "- pos:" << pos << " :" << line.substr(0, pos) << "\n";
        returnArr[i] = stof(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
    }
    if(printVar) cout << "---";
}
vector<string> splitString(string &line, string delimiter, vector<string> &returnVec, bool printVar=false) {
    if(printVar) cout << "--- \"" << line << "\"\n";
    size_t pos = 0;
    vector<string> resultStrings;
    returnVec.clear();
    if(line.find(delimiter)==string::npos) {
        resultStrings.push_back(line);
        returnVec.push_back(line);
        cout << "error: splitString: no delimiter \"" << delimiter << "\" found. Returning empty vector\n";
        return resultStrings;
    }
    while(true) {
        pos = line.find(delimiter);
        if(printVar) cout << "- pos:" << pos << " :" << line.substr(0, pos) << "\n";
        resultStrings.push_back(line.substr(0, pos));
        returnVec.push_back(resultStrings.back());
        line.erase(0, pos + delimiter.length());
        if(line.find(delimiter)==string::npos) {
            resultStrings.push_back(line);
            returnVec.push_back(line);
            break;
        }
    }
    if(printVar) cout << "---";
    return resultStrings;
}


#endif