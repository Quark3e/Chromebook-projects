

#include <iostream>
#include <string>
#include <useful.hpp>
#include <HC_useful/terminalMenu.hpp>



termMenu menu0(true, 1, 3);
bool hardExit = false;


void exitMenu() {
    menu0.exitDriver = true;
    hardExit = true;
}

int main(int argc, char** argv) {
    ANSI_mvprint(50, 1, "TEST:");
    std::string inpStr;


    std::getline(std::cin, inpStr);

    ANSI_mvprint(50, 2, "-----------");

    termMenu menu0(true, 1, 3);
    menu0.addOpt("opt0", 0, 0, 32, exitMenu);
    menu0.addOpt("opt1", 0, 1, -1);
    menu0.addOpt("opt2", 0, 2, -1);

    const int *returPos;

    hardExit = false;

    while(!hardExit) {
        returPos = menu0.driver(1, 1);
    }

    return 0;
}
