
/**
 * @file test0.cpp
 * @author your name (you@domain.com)
 * @brief test heap allocation with smart pointers as a method of valuesVec storage
 * @version 0.1
 * @date 2024-06-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */



#include <iostream>
#include <vector>
#include <memory>

#include <HC_useful/useful.hpp>

class testClass {
private:

    std::vector<std::shared_ptr<std::string>> valuesVec;
    std::vector<std::string> names;
    /**
     * The way the value was added:
     * `0`  - added by value    : allowed to returned as a non-pointer
     * `1`  - added by ptr      : only allowed to be returned as a pointer
     */
    std::vector<int> typeOfAdd;
public:
    testClass(/* args */);
    ~testClass();
    int add(std::string label, std::string input);
    int add(std::string label, std::string* inputPtr);

    int set(std::string label, std::string newValue);
    int set(std::string label, std::string* newPtr);

    std::string* get(std::string label);
    const std::vector<std::string*> get_vector(void);
};

testClass::testClass(/* args */) {}

testClass::~testClass() {
    // for(size_t i=0; i<valuesVec.size(); i++) {
    //     delete valuesVec[i].get();
    // }
}


int testClass::add(std::string label, std::string input) {
    valuesVec.emplace_back(std::make_shared<std::string>(input));
    names.push_back(label);
    typeOfAdd.push_back(0);
    return 0;
}
int testClass::add(std::string label, std::string* inputPtr) {
    valuesVec.emplace_back(inputPtr);
    names.push_back(label);
    typeOfAdd.push_back(1);
    return 0;
}


int testClass::set(std::string label, std::string newValue) {
    int pos = searchVec<std::string>(names, label);
    if(pos==-1) {
        std::cout << "could not find given label"<<std::endl;
        return 1;
    }
    valuesVec[pos].reset(new std::string(newValue));
    typeOfAdd[pos] = 0;
    // valuesVec[pos] = std::make_shared<std::string>(newValue);
    return 0;
}
int testClass::set(std::string label, std::string* newPtr) {
    int pos = searchVec<std::string>(names, label);
    if(pos==-1) {
        std::cout << "could not find given label"<<std::endl;
        return 0;
    }
    valuesVec[pos].reset(newPtr);
    typeOfAdd[pos] = 1;
    return 0;
}


std::string* testClass::get(std::string label) {
    int pos = searchVec<std::string>(names, label);
    if(pos==-1) {
        std::cout << "could not find given label"<<std::endl;
        return nullptr;
    }
    return valuesVec[pos].get();
}
const std::vector<std::string*> testClass::get_vector(void) {
    static std::vector<std::string*> returnVec;
    returnVec.clear();
    for(size_t i=0; i<valuesVec.size(); i++) {
        returnVec.push_back(valuesVec[i].get());
    }
    return returnVec;
}

int main(int argc, char** argv) {
    std::string labels[] = {"s0", "s1", "s2", "s3"};

    testClass obj0;
    obj0.add(labels[0], std::string(""));
    obj0.add(labels[1], std::string("input value"));
    obj0.add(labels[2], labels[2]+"_inp");
    obj0.add(labels[3], labels[3]+"_inp");

    for(int i=0; i<4; i++) {
        std::cout << 
    }


    return 0;
}
