#pragma once
#ifndef H_GET_PERFORMANCE
#define H_GET_PERFORMANCE

#include <iostream>
#include <time.h>
#include <math.h>
#include <vector>

using namespace std;

class getPerf {
    public:
	int currIdx, maxIdx;
    getPerf(int numCheckpoints) {
        cout << "class test:" << numCheckpoints << endl;
    	maxIdx = numCheckpoints;
    //     static clock_t checkPoint_time[numCheckpoints];
    //     static string checkPoint_name[numCheckpoints];
    }
    // void setName(int idx, string check_name) {
    // 	checkPoint_name[idx] = check_name;
    // }
    // void setTime(int check_idx) {
    // 	checkPoints[idx] = clock();
    // }
};

#endif