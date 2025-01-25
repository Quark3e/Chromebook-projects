#include <iostream>
#include <iomanip>
#include <list>
#include <vector>
#include <string>

#include "../../useful/useful.hpp"


int main(int argc, char **argv) {
    std::cout << std::endl;
    
    std::list<std::string> lst;
    for(size_t i=0; i<10; i++) {
        lst.push_back("i="+formatNumber(i, 2, 0));
    }

    auto itr=lst.begin();
    for(size_t i=0; i<10; i++) {
        std::cout << &(*itr)<<" ";
        ++itr;
    }
    std::cout << std::endl;
    for(size_t i=0; i<10; i++) {
        auto itr2=lst.begin();
        std::advance(itr2, i);
        std::cout << &(*itr2) << " ";
    }

    std::cout << std::endl;

    return 0;
}