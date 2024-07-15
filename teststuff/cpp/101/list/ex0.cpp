
#include <iostream>
#include <list>
#include <vector>
#include <string>

std::string nums[10] = {
    "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"
};

template<typename var>
auto itrFunc(const std::list<var>& listRef, size_t idx=0) {
    size_t count = 0;
    for(auto itr = listRef.begin(); itr!=listRef.end(); ++itr) {
        if(count==idx) return itr;
        count+=1;
    }
    return listRef.end();
}


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

    int tempVar=2;
    std::cout <<"["<< tempVar << "]" << " "<<*itrFunc<std::string>(temp, static_cast<size_t>(tempVar));


    

    return 0;
}