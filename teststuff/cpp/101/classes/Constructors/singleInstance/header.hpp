#pragma once
#ifndef H_header
#define H_header

#include <string>
#include <iostream>

using namespace std;

class ParentClass {
    public:
    string var0, var1, var2;
    ParentClass();
    ParentClass(string totalStr);
};


ParentClass::ParentClass() {
    cout << "NOTE: ParentClass initialized------\n";
    var0 = "Default_0";
    var1 = "Default_1";
    var2 = "Default_2";
}
ParentClass::ParentClass(string totStr) {
    cout << "NOTE: ParentClass initialized------\n";
    var0 = totStr+"_0";
    var1 = totStr+"_1";
    var2 = totStr+"_2";
}


#endif