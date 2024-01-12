#pragma once
#ifndef H_CHILDCLASS
#define H_CHILDCLASS


class childClass {
    static ParentClass parentObj;
    public:
    childClass() {};
    childClass(string var) {
    /*constructor stuff*/
    }
    void useChild();
    void test();
};


#endif