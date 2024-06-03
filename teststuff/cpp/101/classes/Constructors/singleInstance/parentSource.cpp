
#include "parentHeader.hpp"


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

