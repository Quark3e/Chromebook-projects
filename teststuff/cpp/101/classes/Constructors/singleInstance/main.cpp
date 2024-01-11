
#include <iostream>
#include <string>
#include "parentHeader.hpp"
#include "childHeader.hpp"

using namespace std;



int main(int argc, char** argv) {
    childClass objs0("test0");
    childClass objs1("test1");
    childClass objs2("test2");
    // childClass objs0("test");

    objs0.useChild();
    objs1.useChild();
    objs2.useChild();



    return 0;
}
