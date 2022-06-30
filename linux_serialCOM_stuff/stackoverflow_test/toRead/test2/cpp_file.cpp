#include "serialHeader.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;
using namespace chrono;

int main() {
    string input;

    Serial::initPort();

    this_thread::sleep_for(milliseconds(1000));
    
    cout << "Enter input:\n 1. Continue\n 2. Exit\n input:";
    cin >> input;
    if(input == "exit" || stoi(input) == 2) {
        return 0;
    }
    for(int i=0;i<7;i++) {
        printf(Serial::getChar());
    }
    return 0;
}