// Example program
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <bits/stdc++.h> 


template<typename T> using vec0 = std::vector<T>; //vector: `T` value
template<typename T> using vec1 = std::vector<std::vector<T>>; //vector: vector of `T` value
template<typename T> using vec2 = std::vector<std::vector<std::vector<T>>>; //vector: vector of vector of `T` value


template<class T>
class cl1 {
    std::vector<T> cl_vec;
    
    public:
    cl1();
    int add(T vecAdd);
    void getType();
};

template<class T> int cl1<T>::add(T vecAdd) {
    cl_vec.push_back(vecAdd);
    return 0;
}

template<class T>cl1<T>::cl1() {}

template<class T> void cl1<T>::getType() {
    std::cout << std::boolalpha;
    if(std::is_same<T, std::string>::value) std::cout << "is std::string" << std::endl;
    else if(std::is_same<T, int>::value) std::cout << "is int" << std::endl;
    else if(std::is_same<T, vec0<int>>::value) std::cout <<"is the using value \"vec0\"" << std::endl;
}



int main() {
    //std::unique_ptr<cl1> obj_int = std::make_unique<cl1>();
    cl1<std::vector<int>> obj_int;
    obj_int.add(std::vector<int>{0, 1, 2});
    obj_int.getType();
    
    return 0;
}