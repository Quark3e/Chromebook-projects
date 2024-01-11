
#include <iostream>
#include <string>
#include "header.hpp"

using namespace std;


class childClass {
  ParentClass &parent_instance;
  public:
  childClass(ParentClass &parent, string var) : parent_instance(parent) {
    /*constructor stuff*/
  }
  void useChild();
};

void childClass::useChild() {
    printf(
        "| %s %s %s |\n",
        parent_instance.var0.c_str(),
        parent_instance.var1.c_str(),
        parent_instance.var2.c_str()
    );
}


int main(int argc, char** argv) {
    childClass objs0(ParentClass);
    childClass objs1(ParentClass);
    childClass objs2(ParentClass);


    // objs0.useChild();
    // objs1.useChild();
    // objs2.useChild();


    return 0;
}
