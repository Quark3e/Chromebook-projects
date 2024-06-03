
#include <iostream>
#include <string>

struct struct0 {
    std::string var0="";

    bool varInit = false;
    std::string func0(int par0) {
        if(!varInit) return "Struct var. was not init.";
        return var0+":"+std::to_string(par0);
    }
} inst1, inst2;


void temp0() {
    // inst1.var0="outside!";
    // inst1.varInit = true;
    inst1 = {
        .var0 = "outside!!!",
        .varInit = true
    };
}

int main(int argc, char** argv) {
    temp0();
    inst2 = {
        .var0 = "test",
        .varInit = true
    };

    std::cout << "-------\n";
    std::cout << inst2.func0(69)<<std::endl;
    std::cout << "-------\n";



    std::cout << "\n-------\n";
    std::cout << inst1.func0(420)<<std::endl;
    std::cout << "-------\n";

    return 0;
}