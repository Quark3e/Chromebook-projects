
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <initializer_list>


template <typename T>
void func(T t) {
    std::cout << "func(T t): "<< t << std::endl;
}

template <typename T, typename... Args>
void func_1(T t, Args... args) {
    std::cout << std::setw(150)<<std::left<<__PRETTY_FUNCTION__<<" : " << t << std::endl;
}

template <typename T, typename... Args>
void func(T t, Args... args) {
    // std::cout << "func(T t, Args... args): "<< t << std::endl;
    
    std::cout << std::setw(145)<<std::left<< __PRETTY_FUNCTION__ << " : " << t <<std::endl;
    func_1(t, args...);
    func(args...);
}

template <class T>
void func2(std::initializer_list<T> list) {
    for(auto elem : list) {
        std::cout << elem << std::endl;
    }
}

int main() {
    std::string str1("Hello"), str2("World");

    func(69, float(2.1), 'a', str1);
    func2({10, 20, 30, 40});
    func2({str1, str2});
}
