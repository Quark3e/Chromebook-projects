#include <iostream>
#include <signal.h>
#include <ncurses.h>
#include <menu.h>
#include <string>
#include <unistd.h>

using namespace std;

#define SCREEN_MAIN 0

#define KKEY_DOWN 258
#define KKEY_UP 259
#define KEY_q 113

char *MenuItems[1024] = {
    "option0",
    "option1",
    "option2"
};
int SelItem = 0;
int LastItem = 0;
char *HeaderText;
char *StatusText;
WINDOW *win;
bool Terminated;
int Screen = 0;
int LastKey = 0;
int MaxX = 0;
int MaxY = 0;

int ProcessScreenMain();
void ClearLine(int y, int l);

int Process() {
    getmaxyx(win, MaxY, MaxX);
    
    switch (Screen) {
        default: ProcessScreenMain();
    }
    
    if (LastKey == 113) {
        Terminated = true;
    }
    else if (LastKey == KEY_EXIT) {
        MenuItems[2] = "69";
        std::cout << "test" << std::endl;
    }
}

int count=0;
int ProcessScreenMain() {
    count++;
    MenuItems[0] = "dffreiodadasdasd";
    MenuItems[1] = "dffreio2222222222";
    LastItem = 2;
    
    attrset(COLOR_PAIR(1));
    for (int i = 0; i <= MaxY; i++) ClearLine(i, MaxX);

    // draw header
    attrset(A_BOLD|COLOR_PAIR(2));
    ClearLine(0, MaxX);
    mvaddstr(0, 0, HeaderText);
    
    // draw body
    attrset(COLOR_PAIR(1));

    for (int i = 0; i <= LastItem; i++) {
        if (SelItem == i) {
        attrset(COLOR_PAIR(3));
        } else {
        attrset(COLOR_PAIR(1));
        }
        ClearLine(1 + i, MaxX);
        // mvaddstr(1 + i, 0, MenuItems[i]);
        MenuItems[i] = (char*)("test"+to_string(count)).c_str();
        mvaddstr(1 + i, 0, MenuItems[i]);
    }

    // draw status line
    attrset(A_BOLD|COLOR_PAIR(2));
    ClearLine(MaxY - 2, MaxX);
    mvaddstr(MaxY - 2, 0, StatusText);

    curs_set(0);
    refresh();
    
    LastKey = getch();
    
    if (LastKey == KEY_UP) SelItem--;
    if (LastKey == KEY_DOWN) SelItem++;

    if (SelItem > LastItem) SelItem = LastItem;
    if (SelItem < 0) SelItem = 0;
}

void ClearLine(int y, int l) {
    move(y, 1);
    l++;
    char Str[l];
    for (int i = 0; i < l; i++) Str[i] = ' ';
    Str[l - 1] = '\0';
    mvaddstr(y, 0, Str);
}

void CatchSIG(int sig) {
    Terminated = true;
}

int main(int argc, char *argv[]) {
    int c = 0;
    
    signal(SIGINT, CatchSIG);
    
    initscr();
    keypad(stdscr, true);
    nonl(); 
    cbreak();
    noecho();   
    win = newwin(0, 0, 0, 0);
    
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE,   COLOR_BLACK);
        init_pair(2, COLOR_GREEN,   COLOR_RED);
        init_pair(3, COLOR_BLACK,   COLOR_GREEN);
    }
    
    SelItem = 0;
    LastItem = 0;
    HeaderText = "  q:Quit  ";
    StatusText = "this is status text";
    
    while (!Terminated) {
        Process();
        usleep(1000);
    }

    cout << "Terminated" << endl;

    endwin();
}