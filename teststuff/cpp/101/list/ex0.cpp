
#include <iostream>
#include <list>
#include <string>

std::string nums[10] = {
    "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"
};

int main(int argc, char** argv) {
    std::list<std::string> temp;
    for(int i=0; i<10; i++) {
        temp.push_back(nums[i]);
    }

    std::cout << "----------" << std::endl;
    
    for(auto it=temp.begin(); it!=temp.end(); ++it) std::cout<< &*it<<"  "<<*it << std::endl;
    std::cout << "----------" << std::endl;

    std::list<std::string>::iterator toRem;
    toRem = temp.begin();
    advance(toRem, 2);
    temp.erase(toRem);

    for(auto it=temp.begin(); it!=temp.end(); ++it) std::cout<< &*it<<"  "<<*it << std::endl;
    std::cout << "----------" << std::endl;

    return 0;
}