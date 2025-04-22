
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#define _TESTMACRO "internal string"
#define _TESTMACRO2 ""

#define XSTRINGLIT(x) STRINGLIT(x)
#define STRINGLIT(x) #x


int main(int argc, char** argv) {
    std::cout << "-----PROGRAM START-----" << std::endl;


    std::cout << "_TESTMACRO:  " << _TESTMACRO << std::endl;
    #ifdef _TESTMACRO2
    // std::cout << "_TESTMACRO2: " << XSTRINGLIT(_TESTMACRO2) << std::endl;
    std::stringstream ss;
    ss << "_TESTMACRO2: " << XSTRINGLIT(_TESTMACRO2);
    std::string strTest = std::string("_TESTMACRO2: ") + std::string(XSTRINGLIT(_TESTMACRO2));

    std::cout << strTest << std::endl;
    #endif //_TESTMACRO2

    std::cout << "-----PROGRAM END  -----" << std::endl;
    return 0;
}