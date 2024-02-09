#include <iostream>

int main() {
    for (int ii=0; ii<10; ++ii ) {
        int input;
        std::cin >> input;
        std::cout << input*ii << std::endl;
        std::cout.flush();
    }
}