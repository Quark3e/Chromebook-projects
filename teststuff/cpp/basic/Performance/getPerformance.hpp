#pragma once
#ifndef H_GET_PERFORMANCE
#define H_GET_PERFORMANCE

#include <iostream>
#include <time.h>
#include <chrono>
#include <ctime>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

/// @brief Class to get performance (fps, delays for each checkpoint, totaldelay)
class getPerf {
    /*
    t0
    -process
    t1; delay = t1-t([1]-1=0)
    
    */
    private:
    int strLenMax = 10; //NOTE: if changed, change update_totalInfo printf

    public:
    float delayFilter = 1;
    bool printNames = true;
    vector<string> names;

    vector<decltype(chrono::steady_clock::now())> times;
    /// @brief vector to hold delays in unit:milliseconds
    vector<float> delays_ms;

    float total_delay;
    float FPS;

    getPerf() {
        names.push_back("t0");
        times.push_back(chrono::steady_clock::now());
        delays_ms.push_back(0);
    }
    void add_checkpoint(string name);
    void update_totalInfo(bool reset_t0,bool printResult,bool printAll,char resultEndSymb0,char resultEndSymb1);
    auto getTime(string name);
    float getDelay(string name);
    string cutStr(string& var, int maxLen);
    int getIdx(string name);
};



/// @brief Function to get clock_t type variable from given checkpoint name
/// @param name the name of checkpoint to get time from
/// @return clock_t type variable
auto getPerf::getTime(string name) {
    int idx=getIdx(name);
    if(idx==-1) {
        printf("error: \"%s\" named checkpoint doesn't exist.\n", name.c_str());
        return chrono::steady_clock::now();
    }
    return times.at(idx);
}



string getPerf::cutStr(string& var, int maxLen=10) {
	int varLen = var.size();
    if(varLen>maxLen) {
        printf(" note: name \"%s\" has been modified to ", var.c_str());
        var.erase(maxLen);
        printf("\"%s\".\n", var.c_str());
    }
    return var;
}


/// @brief Function to get last read delay from given checkpoint name
/// @param name name of checkpoint to get delay from
/// @return float type variable of delay in unit:Milliseconds
float getPerf::getDelay(string name) {
    auto idx=getIdx(name);
    if(idx==-1) {
        printf("error: \"%s\" named checkpoint doesn't exist.\n", name.c_str());
        return 0;
    }
    return delays_ms.at(idx);
}


/// @brief 
/// Creates a time measuring point from previous call. NOTE: call class member AFTER what to measure.
/// if `name` exist then function will not create new element but only update clock_t vector
/// @param name string name of checkpoint to either create or update
void getPerf::add_checkpoint(string name) {
    auto tempTime = chrono::steady_clock::now();
    int idx=getIdx(name);
    if(idx != -1) {
        // If `name` already exist in vector `names
        times.at(idx) = tempTime;
        auto elapsed = chrono::duration_cast<chrono::microseconds>(tempTime-times.at(idx-1));
        // delays_ms.at(idx) = 1000000*(tempTime-times.at(idx-1))/(double)CLOCKS_PER_SEC;
        delays_ms.at(idx) = (elapsed.count()*0.001)*delayFilter+(1.0-delayFilter)*delays_ms.at(idx);
    }
    else {
        printNames = true;
        auto elapsed = chrono::duration_cast<chrono::microseconds>(tempTime-times.back());
        // delays_ms.push_back(1'000'000*(tempTime-times.back())/(double)CLOCKS_PER_SEC);
        delays_ms.push_back((elapsed.count()*0.001));
        times.push_back(tempTime);
        names.push_back(cutStr(name));
    }
    // printf("%7.4f", delays_ms.back());
}

/// @brief update total_delay and FPS member variables
/// @param reset_t0 whether to 
/// @param printResult whether to print total_delay and FPS
/// @param printAll whether to print every checkpoint
void getPerf::update_totalInfo(
    bool reset_t0,
    bool printResult=true,
    bool printAll=true,
    char resultEndSymb0='\n',
    char resultEndSymb1='\n'
) {
    // for(auto n: names) printf(" %5s ", n.c_str());
    // cout << endl;
    // for(auto n: delays_ms) printf(" %5.3f ", n);
    // cout << endl;

    // total_delay = 1000*(times.back()-times.front())/(double)CLOCKS_PER_SEC;

    auto elapsed = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now()-times.front());
    total_delay = (elapsed.count()*0.001)*delayFilter+(1.0-delayFilter)*total_delay;
    FPS = float(1)/(total_delay/1000);
    if(reset_t0) times.at(0) = chrono::steady_clock::now();
    if(printAll) {
        string totalVar = "|", tempS;
        string totalStr = "|";
        for(auto i=0; i<times.size(); i++) {
            if(printNames) {
                string emptySpace(strLenMax-names.at(i).size(), ' ');
                totalStr += names.at(i) + emptySpace + "|";
            }
            stringstream sstream;
            sstream << fixed << setprecision(2) << delays_ms.at(i);
            tempS = sstream.str();
            string emptySpace2(strLenMax-tempS.size()-2, ' ');
            totalVar += emptySpace2 + tempS + "ms" + "|";
        }
        if(printNames) {
            printf("%s\n", totalStr.c_str());
            printNames=false;
        }
        if(printResult) totalVar += resultEndSymb0;
        else totalVar += "\r";
        printf("%s", totalVar.c_str());
    }
    if(printResult) {
        stringstream streamPrint[2];
        streamPrint[0] << fixed << setprecision(2) << FPS;
        streamPrint[1] << fixed << setprecision(2) << total_delay;
        string totStr = "loop iteration info: fps:"+streamPrint[0].str()+" | total_delay:"+streamPrint[1].str()+"ms"+resultEndSymb1; 
        // printf("loop iteration info: fps:%5.2f | total_delay:%6.2fms\r", FPS, total_delay);
        printf("%s",totStr.c_str());
    }
}


int getPerf::getIdx(string name) {
    auto pos = find(names.begin(), names.end(), name);
    if(pos == names.end()) {
        //doesn't exis
        return -1;
    }
    auto idx = distance(names.begin(), pos);
    return idx;
}


#endif