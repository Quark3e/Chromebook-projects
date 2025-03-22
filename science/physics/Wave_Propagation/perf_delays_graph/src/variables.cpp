
#include "variables.hpp"
#include <useful.hpp>
#include <fstream>

pos2d<int> dim__programWindow{1200, 700};


DIY::typed_dict<size_t, DIY::typed_dict<std::string, std::vector<float>>> Loaded_Data__perfDelays({
    
});

void loadCSV_file(std::string _filename) {
    std::ifstream readFile(_filename);
    if(!readFile.is_open()) {
        throw std::runtime_error("ERROR: Failed to open file: \""+_filename+"\"");
        exit(1);
    }
    
    std::string line="";
    if(!std::getline(readFile, line)) {
        throw std::runtime_error("ERROR: Failed to read first line in file.");
        exit(1);
    }

    std::vector<std::string> data_columns;
    splitString(line, ",", &data_columns);
    if(data_columns.size()==0) {
        throw std::runtime_error("ERROR: failed to split column string.");
        exit(1);
    }


    size_t lineCnt = 1;
    while(std::getline(readFile, line)) {
        
    }

}
