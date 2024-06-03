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
#include <fstream>
#include <HC_useful/useful.hpp>

using namespace std;

// / @brief Class to get performance (fps, delays for each checkpoint, totaldelay)
// /
// / 
class getPerf {
    /*
    t0
    -process
    t1; delay = t1-t([1]-1=0)
    
    */
    private:
    /// @brief max number of chars for each checkpoint name
    int strLenMax = 25; //NOTE: if changed, change update_totalInfo printf


    /// @brief whether to save the data into csv file
    bool CSV_save = false;

    /// @brief filename of csv file with path included
    string CSV_filename;

    /// @brief csv file object
    ofstream csvFile;

    int CSV_lineCount = 0;

    bool CSV_init = true;

    public:
    /// @brief filter for new delay values: old_val = new_val*delayFilter + old_val*(1-delayFilter)
    float delayFilter = 1;
    bool printNames = true;
    /// @brief vector to hold names of each checkpoint
    vector<string> names;

    /// @brief vector to hold clock times
    vector<decltype(chrono::steady_clock::now())> times;
    /// @brief vector to hold delays in unit:milliseconds
    vector<float> delays_ms;

    /// @brief total/sum delay in milliseconds of all the checkpoints
    float total_delay;
    /// @brief total FPS from start-end checkpoint
    float FPS;

    /// @brief [0] final print string (don't remember why i ahve two elements and at this point im too afraid to check)
    string rawPrintStrings[2];

    getPerf(string nameInitStr="") {
        names.push_back(nameInitStr+"_t0");
        times.push_back(chrono::steady_clock::now());
        delays_ms.push_back(0);
    }

    /**
     * @brief setup csv related
     * @param filename name of csvfile to create: NOTE: DO NOT ADD `.csv` EXTENSION
     * @param overwrite whether to overwrite `{filename}.csv` if that filename'd file already exists
    */
    void csv_setup(
        string filename,
        bool overwrite
    );
    /**
     * @brief
     * Creates a time measuring point from previous call. NOTE: call class member AFTER what to measure.
     * if `name` exist then function will not create new element but only update clock_t vector
     * @param name string name of checkpoint to either create or update
    */
    void add_checkpoint(string name);
    /**
     * @brief update total_delay and FPS member variables
     * @param reset_t0 whether to 
     * @param printResult whether to print total_delay and FPS
     * @param printAll whether to print every checkpoint
    */
    string update_totalInfo(
        bool reset_t0,
        bool printResult,
        bool printAll,
        char resultEndSymb0,
        char resultEndSymb1
    );
    /**
     * @brief Function to get clock_t type variable from given checkpoint name
     * @param name the name of checkpoint to get time from
     * @return clock_t type variable
    */
    auto getTime(string name);
    /**
     * @brief Function to get last read delay from given checkpoint name
     * @param name name of checkpoint to get delay from
     * @return float type variable of delay in unit:Milliseconds
    */
    float getDelay(string name);
    string cutStr(string& var, int maxLen);
    /**
     * @brief get vector index of name
     * @param name string of the `{name}` to find index of
     * @return index of `{name}`
    */
    int getIdx(string name);
    void close();
};

void getPerf::close() {
    if(CSV_save) {
        csvFile << "# " << getDate();
    

        csvFile.close();
    }
}


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
    if(varLen>=maxLen) {
        // printf(" note: name \"%s\" has been modified to ", var.c_str());
        var.erase(maxLen);
        // printf("\"%s\".\n", var.c_str());
    }
    return var;
}

float getPerf::getDelay(string name) {
    auto idx=getIdx(name);
    if(idx==-1) {
        printf("error: \"%s\" named checkpoint doesn't exist.\n", name.c_str());
        return 0;
    }
    return delays_ms.at(idx);
}


void getPerf::csv_setup(
    string filename,
    bool overwrite=false
) {
    CSV_save = true;
    if(overwrite) {
        csvFile.open(filename+".csv", ios::trunc);
        CSV_filename = filename;
    }
    else {
        int i = 0;
        if(isFile(filename+".csv")) {
            while(true) {
                if(!isFile(filename+"_"+to_string(i)+".csv")) { break; }
                i++;
            }
        }
        csvFile.open(filename+"_"+to_string(i)+".csv");
        CSV_filename = filename+"_"+to_string(i);
    }
    csvFile << "# " << getDate();
}

void getPerf::add_checkpoint(string name) {
    auto tempTime = chrono::steady_clock::now();
    if(name.size()>=strLenMax) name = cutStr(name, strLenMax);
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
        names.push_back(name);
    }
    // printf("%7.4f", delays_ms.back());
}

string getPerf::update_totalInfo(
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

    string totalVar = "|", tempS, totStr;
    if(reset_t0) times.at(0) = chrono::steady_clock::now();
    if(printAll) {
        //prints individual delays
        string totalStr = "|";
        // printf("%d|%d|%d|\n", times.size(), names.size(), delays_ms.size());
        for(auto i=0; i<times.size(); i++) {
            if(printNames) {
                string emptySpace(strLenMax-(names.at(i)).length()+1, ' ');
                totalStr += " " + names.at(i) + emptySpace + "|";
            }
            stringstream sstream;

            sstream << fixed << setprecision(2) << delays_ms.at(i);
            tempS = sstream.str();
            string emptySpace2(strLenMax-tempS.size()-1, ' ');
            totalVar += emptySpace2 + tempS + "ms " + "|";
        }
        rawPrintStrings[0] = totalVar;
        if(printNames) {
            printf("%s\n", totalStr.c_str());
            printNames=false;
        }
        if(printResult) totalVar += resultEndSymb0;
        else totalVar += "\r";
        if(!printResult) printf("%s", totalVar.c_str());
    }
    if(printResult) {
        //prints FPS and total_delay
        stringstream streamPrint[2];
        streamPrint[0] << fixed << setprecision(2) << FPS;
        streamPrint[1] << fixed << setprecision(2) << total_delay;
        totStr = "loop iteration info: fps:"+streamPrint[0].str()+" | total_delay:"+streamPrint[1].str()+"ms"+resultEndSymb1; 
        if(!printAll) printf("%s",totStr.c_str());
    }
    if(printResult && printAll) {
        printf("%s %s", totalVar.c_str(), totStr.c_str());
    }

    if(CSV_save) {
        if(CSV_init) {
            csvFile << "#iter,";
            for(auto i=0; i<names.size(); i++) {
                csvFile << names.at(i);
                if(i<names.size()-1) csvFile << ",";
            }
            csvFile << ",total\n";
            CSV_init = false;
        }
        csvFile << CSV_lineCount << ",";
        for(auto i=0; i<times.size(); i++) {
            csvFile << fixed << setprecision(2) << delays_ms.at(i);
            if(i<times.size()-1) csvFile << ",";
        }
        float sumDelay = 0;
        for(float& n : delays_ms)  sumDelay+=n;
        csvFile << "," << fixed<<setprecision(2)<< sumDelay << "\n";
        CSV_lineCount+=1;
    }
    return "";
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