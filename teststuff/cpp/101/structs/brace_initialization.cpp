
#include <iostream>
#include <string>
#include <vector>


struct brace_initialization {
    std::string var0 = "default 0";
    std::string var1 = "default 1";
    std::string var2 = "default 2";

    int int0 = -1;
    int int1 = -1;
    int int2 = -1;

    brace_initialization() {}
    brace_initialization(int _inp1): int1(_inp1) {}
    brace_initialization(int _inp1, int _inp2): int1(_inp1), int2(_inp2) {}
    brace_initialization(std::string _inp1, std::string _inp2): var1(_inp1), var2(_inp2) {}


    friend auto operator<<(std::ostream &os, brace_initialization const& m) -> std::ostream& {
        os << "\"" << m.var0 << "\" \"" << m.var1 << "\" \"" << m.var2 << "\" | " << m.int0 << " " << m.int1 << " " << m.int2;
        return os;
    }
};


int main(int argc, char** argv) {


    brace_initialization inst0;
    brace_initialization inst1{"one", "two"};
    brace_initialization inst2{69};
    brace_initialization inst3{21, 420};
    std::cout << inst0 << std::endl;
    std::cout << inst1 << std::endl;
    std::cout << inst2 << std::endl;
    std::cout << inst3 << std::endl;

    return 0;
}