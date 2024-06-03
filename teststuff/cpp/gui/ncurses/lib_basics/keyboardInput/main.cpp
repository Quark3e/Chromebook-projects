
#include <iostream>
#include <ncurses.h>
#include <unistd.h>  /* only for sleep() */
#include <thread>

// int kbhit(void)
// {
//     int ch = getch();

//     if (ch != ERR) {
//         ungetch(ch);
//         return ch;
//     } else {
//         return 0;
//     }
// }

int main(void)
{
    initscr();

    cbreak();
    noecho();
    // nodelay(stdscr, TRUE); //make `getch()` non-blocking, meaning it'll return ERR if nothings received
	keypad(stdscr, TRUE); //use keys beyond letters like arrows
    scrollok(stdscr, TRUE);
    int c;
    while ((c=getch())!=27) {
        if(c!=ERR) {
            // printw("Key pressed! It was: %d or %c\n", c, char(c));
            echo();
            mvprintw(0, 0, "Key pressed! It was: %d or %c\n", c, char(c));
            // printf("Key pressed! It was: %d or %c\n", c, char(c));
            if(c==KEY_RIGHT) mvprintw(10, 10, "Right key pressed");
            refresh();
            if(char(c)=='c') {
                std::cout << "\x1B[2J\x1B[2j";
                // printw("\x1B[2J\n");
            }
        }
        // else {
        //     // printw("No key pressed yet...\n");
        //     refresh();
        //     usleep(1'000);
        // }

    }
    endwin();
}