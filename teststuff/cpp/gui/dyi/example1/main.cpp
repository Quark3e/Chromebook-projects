
#include <iostream>
#include <thread>
#include <ncurses.h>
#include <HC_useful/createTable.hpp>
#include <HC_useful/useful.hpp>

//#include "../../../useful/useful.hpp"
//#include "../../../useful/createTable.hpp"


createTable menus[3];

void init_tableMenu() {
    menus[0] = createTable(1, 5);
    menus[1] = createTable(1, 5);
    menus[2] = createTable(1, 5);
    menus[0].add_row();
    for(size_t i=0; i<sizeof(menus)/sizeof(menus[0]); i++) {
        for(size_t row=0; row<menus[i].table.size(); row++) {
            for(size_t col=0; col<menus[i].table[row].size(); col++) {
                menus[i].insertText(
                    " c"+std::to_string(i)+"["+std::to_string(col)+":"+std::to_string(row)+"]",
                    col, row
                );
            }
        }
        menus[i].setColPadding(std::vector<int>(menus[i].table.at(0).size(), 5));
        menus[i].strExport("menu screen: "+std::to_string(i)+"\n",false,"\n"," ");
    }
}


int main(int argc, char** argv) {
    int loopInit=false;
    init_tableMenu();

    initscr();

    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    std::string ansi_code = "\x1B[";

    int currMenu=0;
    int currCell[2] = {0, 0}; /*{x/col, y/row}*/
    int c;

    int tablePos[2] = {5, 5}; /*position/coordinate of table's top-right corner*/

    while((c=getch())!=27) { //getch() is a blocking call
        // if(c!=ERR) {
        if(c==ERR && loopInit) continue;
        if(!loopInit) loopInit=true;
        if(c==KEY_LEFT) {
            currCell[0]--;
            if(currCell[0]<0) currCell[0] = menus[currMenu].table[currCell[1]].size()-1;
        }
        else if(c==KEY_RIGHT) {
            currCell[0]++;
            if(currCell[0]>(menus[currMenu].table[currCell[1]].size()-1)) currCell[0]=0;
        }
        else if(c==KEY_UP) {
            currCell[1]--;
            if(currCell[1]<0) currCell[1] = menus[currMenu].table.size()-1;
        }
        else if(c==KEY_DOWN) {
            currCell[1]++;
            if(currCell[1]>(menus[currMenu].table.size()-1)) currCell[1]=0;
        }
        else if(c==48) { currMenu = 0; }
        else if(c==49) { currMenu = 1; }
        else if(c==50) { currMenu = 2; }
        else if(c==10) {
            currMenu = currCell[0];
        }

        // std::cout << ansi_code+"7;7m";
        ansiPrint(menus[currMenu].exportStr,static_cast<int>(tablePos[0]),static_cast<int>(tablePos[1]));

        int editPos[2] = {0, 0};
        editPos[0] = menus[currMenu].getCellX(currCell[0],currCell[1]);
        editPos[1] = menus[currMenu].getCellY(currCell[0],currCell[1]);

        ANSI_mvprint(
            editPos[0]+tablePos[0],
            editPos[1]+tablePos[1],
            ansi_code+"7;7m"+
            menus[currMenu].table.at(currCell[1]).at(currCell[0])+std::string(menus[currMenu].columnPadding.at(currCell[0]), ' ')
            +ansi_code+"m"
        );

        // ANSI_mvprint(
        //     10, 15, "["+std::to_string(currCell[0])+":"+std::to_string(currCell[1])+"]"
        // );
        // ANSI_mvprint(
        //     10, 16, "["+std::to_string(editPos[0])+":"+std::to_string(editPos[1])+"]"
        // );

        usleep(1'000);
        refresh();
        // }
    }
    endwin();

    return 0;
}
