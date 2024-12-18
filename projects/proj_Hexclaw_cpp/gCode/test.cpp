

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>

#include <useful.hpp>

int main(int argc, char** argv) {
    std::string filename = "ex0.nc";
    std::string line;

    std::ifstream fFile;
    fFile.open(filename);
    if(!fFile.is_open()) {
        perror("Could not open filename");
        exit(1);
    }

    int i=1;
    while(getline(fFile, line)) {
        std::cout << std::setw(3)<<std::left<<i<<": "<<std::setw(40)<<std::left<<line<<"| ";
        std::vector<std::string> splits = splitString(line, " ");
        if(splits.size()==0) continue;
        std::cout << "{ ";
        for(const std::string item: splits) {
            std::cout <<item+", ";
        }
        std::cout << "}";

        std::cout << std::endl;
        i++;
    }

    return 0;
}