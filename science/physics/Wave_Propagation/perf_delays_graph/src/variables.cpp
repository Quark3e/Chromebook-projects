
#include "variables.hpp"
#include <useful.hpp>
#include <fstream>



pos2d<int> dim__programWindow{1200, 700};
pos2d<int> dim__selectionsWindow{200, dim__programWindow.y};
pos2d<int> dim__graphWindow{dim__programWindow.x-dim__selectionsWindow.x, dim__programWindow.y};
pos2d<int> dim__graphRegion{int(double(dim__graphWindow.x)*0.8), int(double(dim__graphWindow.y)*0.8)};

guiwin_nc::imguiwin graphWin(false);
imguigraph graphObj({float(dim__programWindow.x), float(dim__programWindow.y)}, {float(dim__graphWindow.x-dim__graphRegion.x)/2, float(dim__graphWindow.y-(dim__graphWindow.y-dim__graphRegion.y)/2)});


DIY::typed_dict<size_t, DIY::typed_dict<std::string, std::vector<float>>> Loaded_Data__perfDelays;

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

    std::vector<std::string> data_columns_labels;
    splitString(line, ",", &data_columns_labels);
    if(data_columns_labels.size()==0) {
        throw std::runtime_error("ERROR: failed to split column string.");
        exit(1);
    }


    size_t lineCnt = 1;
    while(std::getline(readFile, line)) {
        if(line.size()==0 || line[0]=='\n') continue;

        std::vector<std::string> values;
        splitString(line, ",", &values);
        size_t value_numSubProcess = std::stoul(values[0]); // The primary key. How many threads were created for process_calc
        
        int dict_primaryKeyIdx = Loaded_Data__perfDelays.find_key(value_numSubProcess); // Index to the key:pair that holds this given primary-key/num-threads-for-process_calc
        switch (dict_primaryKeyIdx) {
            case -1: {
                DIY::typed_dict<std::string, std::vector<float>> _tempDict;
                for(size_t i=2; i<data_columns_labels.size(); i++) {
                    _tempDict.add(data_columns_labels[i], {std::stof(values[i])});
                }

                Loaded_Data__perfDelays.add(value_numSubProcess, _tempDict);
            }
            break;
            default: {
                for(size_t i=2; i<data_columns_labels.size(); i++) {
                    Loaded_Data__perfDelays[dict_primaryKeyIdx][i-2].push_back(std::stof(values[i]));
                }
            }
            break;
        }

        lineCnt++;
    }

}
