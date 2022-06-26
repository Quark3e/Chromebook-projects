#include <iostream>
#include <stdio.h>
#include <string>
#include <chrono>
#include <thread>

using namespace std;
using namespace chrono;

int main() {
    
    FILE *file;
    //Opening device file

    int getnum;
    string output = "";
    
    file = fopen("/dev/ttyUSB0", "r+");

    while (true) {
        // fprintf(file, "%d\n", getnum); //Writing to the file
        fscanf(file, "%s", output);
        rewind(file);
        printf(">> %s\n", output);
        this_thread::sleep_for(milliseconds(100));     
    }

    fclose(file);
}