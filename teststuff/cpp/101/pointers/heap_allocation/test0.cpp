
/**
 * @file test0.cpp
 * @author Quark3e (you@domain.com)
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

using std::cout, std::endl;

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

    int edit(std::string label, std::string newValue);

    std::string* getPtr(std::string label);
    const std::vector<std::string*> getPtr_vector(void);
    std::vector<std::string> getNames_vector(void) { return names; }
};

testClass::testClass(/* args */) {}

testClass::~testClass() {}


int testClass::add(std::string label, std::string input) {
    valuesVec.emplace_back(std::make_shared<std::string>(input));
    names.push_back(label);
    typeOfAdd.push_back(0);
    return 0;
}
int testClass::add(std::string label, std::string* inputPtr) {
    valuesVec.emplace_back(std::shared_ptr<std::string>(inputPtr, [](std::string*){}));
    names.push_back(label);
    typeOfAdd.push_back(1);
    return 0;
}


int testClass::set(std::string label, std::string newValue) {
    int pos = searchVec<std::string>(names, label);
    if(pos==-1) {
        cout << "could not find given label"<< endl;
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
        cout << "could not find given label"<< endl;
        return 1;
    }
    valuesVec[pos].reset();
    valuesVec[pos] = std::shared_ptr<std::string>(newPtr, [](std::string*){});
    typeOfAdd[pos] = 1;
    return 0;
}

int testClass::edit(std::string label, std::string newValue) {
    int pos = searchVec<std::string>(names, label);
    if(pos==-1) {
        cout << "could not find given label" << endl;
        return 1;
    }
    switch (typeOfAdd[pos]) {
    case 0:
        valuesVec[pos].reset(new std::string(newValue));
        break;
    case 1:
        *(valuesVec[pos].get()) = newValue;
    default:
        return 1;
        break;
    }
    return 0;
}


std::string* testClass::getPtr(std::string label) {
    int pos = searchVec<std::string>(names, label);
    if(pos==-1) {
        cout << "could not find given label"<< endl;
        return nullptr;
    }
    return valuesVec[pos].get();
}
const std::vector<std::string*> testClass::getPtr_vector(void) {
    static std::vector<std::string*> returnVec;
    returnVec.clear();
    for(size_t i=0; i<valuesVec.size(); i++) {
        returnVec.push_back(valuesVec[i].get());
    }
    return returnVec;
}



int main(int argc, char** argv) {
    std::string labels[] = {"s0", "s1", "s2", "s3"};
    std::string local0 = "local 0", local1 = "local 1";

    testClass obj0;
    obj0.add(labels[0], std::string("()"));
    obj0.add(labels[1], std::string("input value"));
    obj0.add(labels[2], &local0);
    obj0.add(labels[3], labels[3]+"_inp");


    for(std::string* ptr: obj0.getPtr_vector())
        cout << "»"<< *ptr <<  endl;

    cout <<  endl;
    cout << "local0: " << local0 <<endl;
    cout << "local1: " << local1 <<endl;
    cout << "------" <<  endl;

    obj0.set(labels[3], &local1);
    obj0.set(labels[0], std::string("label0 test 2()"));

    for(std::string* ptr: obj0.getPtr_vector())
        cout << "»"<< *ptr <<  endl;

    obj0.edit(labels[2], "from local");

    cout <<  endl;
    cout << "local0: " << local0 <<endl;
    cout << "local1: " << local1 <<endl;


    cout << "-----------"<< endl;
    return 0;
}
