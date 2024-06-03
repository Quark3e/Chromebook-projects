
#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include "../../../useful/useful.hpp"


std::string termStr = "\x1B[";


void mvprint(
    int posx,
    int posy,
    std::string printStr,
    bool endline=true,
    bool flushEnd=true
) {
    std::cout << termStr<<posx<<":"<<posy<<"H"<<printStr;
    if(endline) std::cout << "\n";
    if(flushEnd) std::cout.flush();
}

int main(int argc, char** argv) {


    return 0;
}
