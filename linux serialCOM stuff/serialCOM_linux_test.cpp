#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;

int main() {
    int data[] = {69, 420, 112};
    FILE *file;
    file = fopen("/dev/ttyUSB0", "w");
    
    for(int i=0; i<3; i++) {
        fprintf(file, "%d", data[i]); //Writing to the file
        fprintf(file, "%c", ','); //To separate digits
        this_thread::sleep_for(chrono::milliseconds(1));
    }
    fclose(file);
}
