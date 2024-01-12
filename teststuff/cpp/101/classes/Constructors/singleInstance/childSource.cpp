
#include "parentHeader.hpp"
#include "childHeader.hpp"

ParentClass childClass::parentObj{/*pre-defined value*/};

void childClass::test() {
    printf("test\n");
}

void childClass::useChild() {
    printf(
        "| %s %s %s |\n",
        parentObj.var0.c_str(),
        parentObj.var1.c_str(),
        parentObj.var2.c_str()
    );
}