
#include <iostream>
#include <terminalMenu.hpp>
#include <useful.hpp>

struct int2 {
    int x;
    int y;
    int operator[](size_t i) {
        switch (i) {
        case 0: return x;
        case 1: return y;
        default:
            break;
        }
        throw std::invalid_argument("Invalid argument.");
    }
};


void termMenu_call();

int2 selectedPos{0, 0};

int main(int argc, char** argv) {
    termMenu menu(true, 4, 4);
    // menu.addOpt("(option 0:0)", 0, 0, -1, termMenu_nullFunc_void__);
    for(size_t x=0; x<4; x++) {
        for(size_t y=0; y<4; y++) {
            menu.addOpt("(option: "+formatNumber(x,2,0)+","+formatNumber(y,2,0)+")", x, y, -1, termMenu_call);
        }
    }

    const int *pressed_pos;
    while(true) {
        pressed_pos = menu.driver(10, 10, 0);
        selectedPos.x = pressed_pos[0];
        selectedPos.y = pressed_pos[1];
        termMenu_call();
    }

    return 0;
}


void termMenu_call() {
    std::cout << formatContainer1(selectedPos, 2, 0, 0, "left") << std::endl;
}