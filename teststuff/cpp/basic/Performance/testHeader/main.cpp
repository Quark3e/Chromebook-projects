
#include <iostream>
#include <unistd.h
#include "../getPerformance.hpp"

using namespace std;


int main(int argc, char** argv) {
    // for(int i=0; i<argc; i++) cout << argv[i] << endl;
    usleep();
    getPerf perfObj(stoi(argv[1]));
    cout << "test" << endl;

    return 0;
}