
#include <iostream>
#include <time.h>
#include <math.h>
#include <cmath>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <chrono>

#include "../../useful/useful.hpp"

using namespace std;


int main(int argc, char** argv) {
    srand(time(NULL));

    for(int i=0; i<10'010; i+=1) {
        progressBar(int(i), 10'000.0, true, false, 0.1);
        // usleep(1'5000+(rand()%10+1)*1'00);
        usleep(10'000);
    }
    progressBar(10'000.0, 10'000.0, true, true);
    cout << endl;

}