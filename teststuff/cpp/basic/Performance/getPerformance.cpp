
#include "getPerformance.hpp"



getPerf::getPerf(
    const getPerf& _other
):
strLenMax(_other.strLenMax), CSV_save(_other.CSV_save), CSV_filename(_other.CSV_filename),
CSV_lineCount(_other.CSV_lineCount), CSV_init(_other.CSV_init),
delayFilter(_other.delayFilter), printNames(_other.printNames), names(_other.names), times(_other.times), delays_ms(_other.delays_ms),
total_delay(_other.total_delay), FPS(_other.FPS) {
    // csvFile = _other.csvFile;

}
getPerf::getPerf(std::string nameInitStr) {
    names.push_back(nameInitStr+"_t0");
    times.push_back(std::chrono::steady_clock::now());
    delays_ms.push_back(0);
}
getPerf::~getPerf() {

}

getPerf& getPerf::operator=(const getPerf& _other) {
    strLenMax   = _other.strLenMax;
    CSV_save    = _other.CSV_save;
    CSV_filename    = _other.CSV_filename;
    CSV_lineCount   = _other.CSV_lineCount;
    CSV_init    = _other.CSV_init;
    delayFilter = _other.delayFilter;
    printNames  = _other.printNames;
    names   = _other.names;
    times   = _other.times;
    delays_ms   = _other.delays_ms;
    total_delay = _other.total_delay;
    FPS = _other.FPS;

    return *this;
}

void getPerf::close() {
    if(CSV_save) {
        csvFile << "# " << getDate();
    

        csvFile.close();
    }
}

auto getPerf::getTime(std::string name) {
    int idx=getIdx(name);
    if(idx==-1) {
        printf("error: \"%s\" named checkpoint doesn't exist.\n", name.c_str());
        return std::chrono::steady_clock::now();
    }
    return times.at(idx);
}

std::string getPerf::cutStr(std::string& var, int maxLen) {
	int varLen = var.size();
    if(varLen>=maxLen) {
        // printf(" note: name \"%s\" has been modified to ", var.c_str());
        var.erase(maxLen);
        // printf("\"%s\".\n", var.c_str());
    }
    return var;
}

float getPerf::getDelay(std::string name) {
    auto idx=getIdx(name);
    if(idx==-1) {
        printf("error: \"%s\" named checkpoint doesn't exist.\n", name.c_str());
        return 0;
    }
    return delays_ms.at(idx);
}


void getPerf::csv_setup(
    std::string filename,
    bool overwrite
) {
    CSV_save = true;
    if(overwrite) {
        csvFile.open(filename+".csv", std::ios::trunc);
        CSV_filename = filename;
    }
    else {
        int i = 0;
        if(isFile(filename+".csv")) {
            while(true) {
                if(!isFile(filename+"_"+std::to_string(i)+".csv")) { break; }
                i++;
            }
        }
        csvFile.open(filename+"_"+std::to_string(i)+".csv");
        CSV_filename = filename+"_"+std::to_string(i);
    }
    csvFile << "# " << getDate();
}

void getPerf::add_checkpoint(std::string name) {
    auto tempTime = std::chrono::steady_clock::now();
    if(name.size()>=strLenMax) name = cutStr(name, strLenMax);
    int idx=getIdx(name);
    if(idx != -1) {
        // If `name` already exist in vector `names
        times.at(idx) = tempTime;
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(tempTime-times.at(idx-1));
        // delays_ms.at(idx) = 1000000*(tempTime-times.at(idx-1))/(double)CLOCKS_PER_SEC;
        delays_ms.at(idx) = (elapsed.count()*0.001)*delayFilter+(1.0-delayFilter)*delays_ms.at(idx);
    }
    else {
        printNames = true;
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(tempTime-times.back());
        // delays_ms.push_back(1'000'000*(tempTime-times.back())/(double)CLOCKS_PER_SEC);
        delays_ms.push_back((elapsed.count()*0.001));
        times.push_back(tempTime);
        names.push_back(name);
    }
    // printf("%7.4f", delays_ms.back());
}

std::string getPerf::update_totalInfo(
    bool reset_t0,
    bool printResult,
    bool printAll,
    char resultEndSymb0,
    char resultEndSymb1
) {
    // for(auto n: names) printf(" %5s ", n.c_str());
    // cout << endl;
    // for(auto n: delays_ms) printf(" %5.3f ", n);
    // cout << endl;

    // total_delay = 1000*(times.back()-times.front())/(double)CLOCKS_PER_SEC;

    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-times.front());
    total_delay = (elapsed.count()*0.001)*delayFilter+(1.0-delayFilter)*total_delay;
    FPS = float(1)/(total_delay/1000);

    std::string totalVar = "|", tempS, totStr;
    if(reset_t0) times.at(0) = std::chrono::steady_clock::now();
    if(printAll) {
        //prints individual delays
        std::string totalStr = "|";
        // printf("%d|%d|%d|\n", times.size(), names.size(), delays_ms.size());
        for(auto i=0; i<times.size(); i++) {
            if(printNames) {
                std::string emptySpace(strLenMax-(names.at(i)).length()+1, ' ');
                totalStr += " " + names.at(i) + emptySpace + "|";
            }
            std::stringstream sstream;

            sstream << std::fixed << std::setprecision(2) << delays_ms.at(i);
            tempS = sstream.str();
            std::string emptySpace2(strLenMax-tempS.size()-1, ' ');
            totalVar += emptySpace2 + tempS + "ms " + "|";
        }
        rawPrintStrings[0] = totalVar;
        if(printNames) {
            printf("%s\n", totalStr.c_str());
            printNames=false;
        }
        if(printResult) totalVar += resultEndSymb0;
        else totalVar += "\r";
        if(!printResult) printf("%s", totalVar.c_str());
    }
    if(printResult) {
        //prints FPS and total_delay
        std::stringstream streamPrint[2];
        streamPrint[0] << std::fixed << std::setprecision(2) << FPS;
        streamPrint[1] << std::fixed << std::setprecision(2) << total_delay;
        totStr = "loop iteration info: fps:"+streamPrint[0].str()+" | total_delay:"+streamPrint[1].str()+"ms"+resultEndSymb1; 
        if(!printAll) printf("%s",totStr.c_str());
    }
    if(printResult && printAll) {
        printf("%s %s", totalVar.c_str(), totStr.c_str());
    }

    if(CSV_save) {
        if(CSV_init) {
            csvFile << "#iter,";
            for(auto i=0; i<names.size(); i++) {
                csvFile << names.at(i);
                if(i<names.size()-1) csvFile << ",";
            }
            csvFile << ",total\n";
            CSV_init = false;
        }
        csvFile << CSV_lineCount << ",";
        for(auto i=0; i<times.size(); i++) {
            csvFile << std::fixed << std::setprecision(2) << delays_ms.at(i);
            if(i<times.size()-1) csvFile << ",";
        }
        float sumDelay = 0;
        for(float& n : delays_ms)  sumDelay+=n;
        csvFile << "," << std::fixed<<std::setprecision(2)<< sumDelay << "\n";
        CSV_lineCount+=1;
    }
    return "";
}

int getPerf::getIdx(std::string name) {
    auto pos = find(names.begin(), names.end(), name);
    if(pos == names.end()) {
        //doesn't exis
        return -1;
    }
    auto idx = distance(names.begin(), pos);
    return idx;
}
