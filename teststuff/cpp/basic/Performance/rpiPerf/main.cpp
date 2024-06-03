/**
 * Graph clock speed and print cmds of checkPerf.sh in human readable form
 * with ansi print
*/

#define graphRES    100
#define graphHeight 20
#define min_freq    0 //hard defined
#define max_freq    2'200'000'000 //hard defined

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <thread>
#include <chrono>

#include "../../../useful/useful.hpp"


std::string cmds[] = {
    "vcgencmd measure_clock arm",
    "vcgencmd measure_temp",
    "vcgencmd measure_volts",
    "vcgencmd get_throttled",
    "df -h",
    "free -h"
};

std::string cmdResult[6] = {
    "",
    "",
    "",
    "",
    "",
    ""
};
std::string outGraph = "";


unsigned int measured_clockFreqs[graphRES];
int measured_temps[graphRES];
float measured_voltages[graphRES];

/// @brief dynamic min- max limits
float dynamicLim[2] = {0, 1};

/**
 * Which measured value to graph
*/
int graphMode=0;
/// @brief sleep duration in ms
int iterSleep=0;

int termSize[2] = {0, 0};


void updateGraph();

int main(int argc, char** argv) {
    if(argc==2) {
        std::string arg0 = argv[1];
        if(arg0.substr(0,2)=="-d") iterSleep = stoi(arg0.substr(2,arg0.length()));
        if(arg0.substr(0,3)=="-m0") graphMode = 0;
        if(arg0.substr(0,3)=="-m1") graphMode = 1;
        if(arg0.substr(0,3)=="-m2") graphMode = 2;
        if(arg0.substr(0,3)=="-m3") graphMode = 3;
        if(arg0.substr(0,3)=="-m4") graphMode = 4;
        if(arg0.substr(0,3)=="-m5") graphMode = 5;
    }

    signal(SIGINT, signal_callback_handler);
    for(int i=0; i<graphRES; i++) {
        measured_clockFreqs[i] = 0;
    }
    getTermSize(termSize[0], termSize[1]);

    while(true) {
        updateGraph();
        std::string totalString="";
        for(int i=0; i<sizeof(cmds)/sizeof(cmds[0]); i++) {
            if(i==1) {
                // std::string currFreq="";
                totalString += formatNumber(static_cast<double>(measured_clockFreqs[graphRES-1])/1'000'000'000,5,3) + "GHz ";
                totalString += formatNumber(static_cast<double>(measured_clockFreqs[graphRES-1])/max_freq*100,5,1) + "%\n";
            }
            totalString+=cmdResult[i];
            totalString+="\n";
        }
        ansiPrint(totalString, 0, 0, true, true);

        std::this_thread::sleep_for(std::chrono::milliseconds(iterSleep));
    }

    return 0;
}

void updateGraph() {
    getTermSize(termSize[0], termSize[1]);

    for(int i=0; i<sizeof(cmds)/sizeof(cmds[0]); i++) {
        cmdResult[i] = cmdExec(cmds[i].c_str(), false);
    }

    for(int i=0; i<graphRES-1; i++) {
        measured_clockFreqs[i] = measured_clockFreqs[i+1];
    }
    // std::cout << cmdResult[0].substr(14, cmdResult[0].length()-14) << std::endl;
    measured_clockFreqs[graphRES-1] = stoul(
        cmdResult[0].substr(
            14,
            cmdResult[0].length()-14
        )
    );
    // dynamicLim[0] = findVal(..)
    cmdResult[0] = "";
    std::string resultStr = "";
    double xScal = static_cast<double>(termSize[0])/static_cast<float>(graphRES);
    double yScal = static_cast<double>(graphHeight)/static_cast<double>(max_freq);
    for(int row=0; row<graphHeight; row++) {
        for(int x=0; x<graphRES; x++) {
            if(round(yScal*static_cast<double>(measured_clockFreqs[x])>graphHeight-row)) {
                resultStr += std::string(xScal, 'O');
            }
            else {
                resultStr += std::string(static_cast<int>(round(xScal)),'_');
            }
        }
        resultStr += "\n";
    }
    cmdResult[0] = resultStr;
}