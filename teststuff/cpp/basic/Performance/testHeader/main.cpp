
#include <iostream>
#include <unistd.h>
#include <string>
#include "../getPerformance.hpp"

using namespace std;

string test[10] = {
    "zero"
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
    getPerf perfObj();



    usleep(1'000'000);
    

    return 0;
}