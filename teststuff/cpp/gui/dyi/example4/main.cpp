
#include <iostream>
#include <iomanip>
#include <conio.h>

int main(int argc, char** argv) {
    int ch = 0;
    while((ch = getch())!=27) {
        std::cout << std::setw(3) << ch << "  : ";
        if(ch == 224) { // if the first value is esc
            // getch(); // skip the [
            switch(getch()) { // the real value
                case 72:
                    // code for arrow up
                    std::cout << "arrow_up";
                    break;
                case 80:
                    // code for arrow down
                    std::cout << "arrow_down";
                    break;
                case 77:
                    // code for arrow right
                    std::cout << "arrow_right";
                    break;
                case 75:
                    // code for arrow left
                    std::cout << "arrow_left";
                    break;
            }
        }
        else {
            std::cout << char(ch);
        }
        std::cout << std::endl;
    }


    return 0;
}