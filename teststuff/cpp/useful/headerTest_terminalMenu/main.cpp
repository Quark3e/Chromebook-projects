
#include <iostream>
#include <string>
#include <terminalMenu.hpp>
#include <useful.hpp>
#include <chrono>

#include <curses.h>

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
    int2 operator=(int2& m) {
        int2 temp{m.x, m.y};
        this->x = m.x;
        this->y = m.y;
        return temp;
    }
    int2 operator=(int _ptr[2]) {
        int2 temp{_ptr[0], _ptr[1]};
        this->x = temp.x;
        this->y = temp.y;
        return temp;
    }
};


void termMenu_call();
void loop_call();
void exit_menu();

int2 selectedPos{0, 0};
int2 hoveredPos{0, 0};

TUI::termMenu menu(true, 4, 4);

bool running = true;
int main(int argc, char** argv) {
    // menu.addOpt("(option 0:0)", 0, 0, -1, termMenu_nullFunc_void__);

    for(size_t x=0; x<4; x++) {
        for(size_t y=0; y<4; y++) {
            menu.addOpt("(option: "+formatNumber(x,2,0)+","+formatNumber(y,2,0)+")", x, y, -1, termMenu_call);
        }
    }
    menu.addOpt("exit", 0, 5, 27, exit_menu);
    
    menu.driver(2, 2, 10, true, loop_call);
    // const int *pressed_pos;
    // while(running) {
    //     pressed_pos = menu.driver(10, 10, 0, true, loop_call);
    //     selectedPos.x = pressed_pos[0];
    //     selectedPos.y = pressed_pos[1];
    //     termMenu_call();
    // }

    return 0;
}


void termMenu_call() {
    // std::cout << formatContainer1(selectedPos, 2, 0, 0, "left") << std::endl;
}
void loop_call() {
    hoveredPos = menu.currCell;
    selectedPos= menu.pressedCell;
    std::string outStr = "hovered"+formatContainer1(hoveredPos, 2, 2, 0)+" selected"+formatContainer1(selectedPos, 2, 2, 0);
    ANSI_mvprint(0, 0, outStr);
}
void exit_menu() {
    menu.exitDriver = true;
}