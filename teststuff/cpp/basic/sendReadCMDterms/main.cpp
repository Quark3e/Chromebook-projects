
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

std::string exec(const char* cmd, bool removeNewLine=true) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if(removeNewLine && result[result.length()-1]=='\n') result.pop_back();

    return result;
}

int main(int argc, char** argv) {
    std::string testArg = "free -h";

    std::cout <<"|"<< exec(testArg.c_str()) <<"|"<< std::endl;

    return 0;
}
