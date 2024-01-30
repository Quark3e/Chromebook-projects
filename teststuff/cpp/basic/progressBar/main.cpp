
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

    for(int i=0; i<10000; i++) {
        progressBar(float(i), 10000, true);
        // usleep(1'5000+(rand()%10+1)*1'00);
        usleep(1'500);
    }
    cout << endl;

}