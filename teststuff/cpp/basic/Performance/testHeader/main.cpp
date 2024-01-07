
#include <iostream>
#include <unistd.h>
#include <string>
#include <math.h>
#include "../getPerformance.hpp"

using namespace std;

string test[10] = {
    "zero",
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
};

// for(int i=0; i<argc; i++) cout << argv[i] << endl;
int main(int argc, char** argv) {
    getPerf perfObj;


    cout << "loop starting\n";
    while(true) {
        for(int i=0; i<sizeof(test)/sizeof(test[0]); i++) {
            usleep(420*i+69);
            perfObj.add_checkpoint(test[i]);
        }
        // usleep(420'690);
        perfObj.update_totalInfo(true, true, true, ' ');
    }

    return 0;
}