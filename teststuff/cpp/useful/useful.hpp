
#pragma once
#ifndef H_USEFUL
#define H_USEFUL

#include <math.h>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <chrono>
#include <ctime>

#include <algorithm>

#include <cstring>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <linux/limits.h>

// using namespace std;


/// @brief return the sum of elements in an array of type `T`
/// @tparam T 
/// @param arr pointer to the array (array name) to find sum of
/// @return the sum of all elements in `T` data type
template<class T>
double array_sum(T *arr, int arrSize) {
    double sum = 0;
    for(int i=0; i<arrSize; i++) sum += arr[i];
    return sum;
}
template<class T>
double array_sum(T vec) {
    double sum = 0;
    for(size_t i=0; i<vec.size(); i++) sum += vec.at(i);
    return sum;
}

// int array_sum(int *arr) {
//     int sum=0;
//     for(int i=0; i<sizeof(arr)/sizeof(arr[0]); i++) sum += arr[i];
//     return sum;
// }
// float array_sum(float *arr) {
//     float sum=0;
//     for(int i=0; i<sizeof(arr)/sizeof(arr[0]); i++) sum += arr[i];
//     return sum;
// }
// double array_sum(double *arr) {
//     double sum;
//     for(int i=0; i<sizeof(arr)/sizeof(arr[0]); i++) sum += arr[i];
//     return sum;
// }

bool array_bool_AND(bool *arrCheck, int arrSize) {
    for(int i=0; i<arrSize; i++) {
        if(!arrCheck[i]) return false;
    }
    return true;
}
bool array_bool_OR(bool *arrCheck, int arrSize) {
    for(int i=0; i<arrSize; i++) {
        if(arrCheck[i]) return true;
    }
    return false;
}


bool isNumber(const char* stringToCheck) {
    const char* numbersChar = "0123456789";
    const char* numbersChar_associated = "-+.";
	bool isNum=false;
	for(int i=0; i<10; i++) {
		if(stringToCheck[0]==numbersChar[i]) {
			isNum=true;
			break;
		}
        if(
            stringToCheck[0]==numbersChar_associated[0] || 
            stringToCheck[0]==numbersChar_associated[1] ||
            stringToCheck[0]==numbersChar_associated[2]
        ) {
            bool subBreak = false;
            for(int ii=0; ii<10; ii++) {
                if(stringToCheck[1]==numbersChar[ii]) {
                    isNum=true;
                    break;
                }
            }
            break;
        }
	}
	return isNum;
}

bool isNumber(std::string stringToCheck) {
    std::string numbersChar = "0123456789";
    std::string numbersChar_associated = "-+.";
    bool isNum=false;
    for(int i=0; i<10; i++) {
		if(stringToCheck[0]==numbersChar[i]) {
			isNum=true;
			break;
		}
        if(
            stringToCheck[0]==numbersChar_associated[0] || 
            stringToCheck[0]==numbersChar_associated[1] ||
            stringToCheck[0]==numbersChar_associated[2]
        ) {
            bool subBreak = false;
            for(int ii=0; ii<10; ii++) {
                if(stringToCheck[1]==numbersChar[ii]) {
                    isNum=true;
                    break;
                }
            }
            break;
        }
	}
    return isNum;
}

/// @brief execute command line argument and return result
/// @param cmd command std::string to execute
/// @param removeNewLine whether to remove trailing newline from cmd output
/// @return result std::string of the command
std::string cmdExec(const char* cmd, bool removeNewLine=true) {
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

/// @brief find number of char occurances in a string
/// @param toCheck string to find occurrences in
/// @param toFind char to find occurrences
/// @return number of occurrences
int findOcc_char(std::string toCheck, char toFind) {
    std::string::difference_type n = std::count(toCheck.begin(), toCheck.end(), toFind);
    return static_cast<int>(n);
}


#include <cstdlib>
#include <signal.h>
/**
 * detect signal from callback
 * @param signum signal to catch on callback
 * @note to detect KeyboardInterrupt pass `signal(SIGINT, signal_callback_handler);` at the start of the program before the loop
*/
void signal_callback_handler(int signum) {
   std::cout <<std::endl<< "Caught signal " << signum << std::endl;
   // Terminate program
   exit(signum);
}
// #include "createTable.hpp"

/**
 * @brief search and find index in std::string vector to given std::string to find
 * @param vec `std::std::vector<std::string>` vector search through
 * @param toFind `std::string` var to find in vector
 * @return index in vector, or `-1` if not found
*/
int findVectorIndex(std::vector<std::string> vec, std::string toFind) {
    std::vector<std::string>::iterator idx = find(vec.begin(), vec.end(), toFind);

    if(idx != vec.end()) {
        return idx-vec.begin();
    }
    else {
        return -1;
    }
}
/**
 * @brief search and find index in std::string vector to given std::string to find
 * @param vecvec `std::std::vector<std::std::vector<std::string>>` vector with vector search through
 * @param toFind `std::string` var to find in vector
 * @return 2d `std::std::vector<int>` of position/coordinates
*/
std::vector<int> findVectorIndex(std::vector<std::vector<std::string>> vecvec, std::string toFind, bool printChecks=false) {
    if(printChecks) {
        std::cout <<"\"" <<toFind << "\": "<< vecvec.size()<< ": {\n";
        for(std::vector<std::string> vecStr: vecvec) {
            std::cout << "\t{ ";
            for(std::string strIIng: vecStr) std::cout << strIIng << " ";
            std::cout<<"}" << std::endl;
        }
        std::cout << "}"<<std::endl;
    }
   
    if(vecvec.size()==0 || vecvec[0].size()==0) return std::vector<int>(2, -1);

    std::vector<int> indices(2, -1);
    for(size_t i=0; i<vecvec.size(); i++) {
        for(size_t n=0; n<vecvec[i].size(); n++) {
            if(vecvec[i][n]==toFind) {
                indices[0] = i;
                indices[1] = n;
                return indices;
            }
        }
    }
    return indices;
}
std::vector<int> findVectorIndex(std::vector<std::vector<std::string>> vecvec, std::vector<std::string> toFind, bool printChecks=false) {
    std::vector<int> indices(2, -1);
    for(std::string subFind: toFind) {
        indices = findVectorIndex(vecvec, subFind, printChecks);
        if(indices[0] != -1) return indices;
    }
    return indices;
}


/**
 * @brief find desired value from array
 * @param arrToCheck the array to look throug
 * @param arrLen length of the array
 * @param mode which-mode/what-to-look-for.
 * `0`- biggest value
 * `1`- smallest value
 * `2`- index of biggest value
 * `3`- index of smallest value
 * @return return desired value
*/
float findVal(float arrToCheck[], int arrLen, int mode=0) {
    /*
    mode:
    - 0 - biggest, value
    - 1 - smallest, value
    - 2 - biggest, index
    - 3 - smallest, index
    */
    float val = arrToCheck[0];
    int index = 0;
    for(int i=0; i<arrLen; i++) {
        if(mode==0 || mode==2) if(arrToCheck[i]>val) { val=arrToCheck[i]; index=i; }
        else if(mode==1 || mode==3) if(arrToCheck[i]<val) {val=arrToCheck[i]; index=i; }
    }
    if(mode==0 || mode==1) return val;
    else if(mode==2 || mode==3) return index;
    else return -1;
}

static std::string dummyRef_stringOfVector_resultStrRef;
/**
 * @brief find specific std::string from `std::std::vector<std::string>` parameter
 * @param stringVec `std::std::vector<std::string>` find find specific std::string from
 * @param id id for what to look for:
 * `0` - find shortest string;
 * `1` - find longest string;
 * @return index of the std::string in the vector
*/
int stringOfVector(
    std::vector<std::string> stringVec,
    int id,
    std::string& resultStrRef = dummyRef_stringOfVector_resultStrRef
) {
    int idx;
    std::vector<std::string>::iterator leWord;

    if(id == 0) {
        leWord = min_element(
            stringVec.begin(), stringVec.end(),
            [](const auto& a, const auto& b) {
                return a.size() < b.size();
            });
    }
    else if(id == 1) {
        leWord = max_element(
            stringVec.begin(), stringVec.end(),
            [](const auto& a, const auto& b) {
                return a.size() < b.size();
            });
    }
    else {
        std::cout << "ERROR: stringOfVector: wrong id selected. Exiting.." << std::endl;
        return -1;
    }
    resultStrRef = *leWord;
    return distance(stringVec.begin(), leWord);
}

void splitString(std::string line, std::string delimiter, float returnArr[], int numVar, bool printVar);

/**
 * @brief Split and return `line` std::string by each `delimiter`
 * @param line std::string to split.
 * @param delimiter std::string for where to split the std::string `line` by.
 * @param printVar whether to print each delimiter and substring
*/
std::vector<std::string> splitString(std::string line, std::string delimiter, bool printVar);
/**
 * @brief Split `line` std::string by each `delimiter` and pass the result to reference parameter `returnVec`
 * @param line std::string to split.
 * @param delimiter std::string for where to split the std::string `line` by.
 * @param returnVec reference to a `std::vector<std::string>` for which to pass result to
 * @param printVar whether to print each delimiter and substring
*/
void splitString(std::string line, std::string delimiter, std::vector<std::string> &returnVec, bool printVar);
/**
 * @brief Split `line` std::string by each `delimiter` and pass the result to a pointer parameter `returnPtr`
 * @param line std::string to split.
 * @param delimiter std::string for where to split the std::string `line` by.
 * @param returnPtr pointer to a `std::vector<std::string>` for which to pass result to
 * @param printVar whether to print each delimiter and substring
*/
void splitString(std::string line, std::string delimiter, std::vector<std::string> *returnPtr, bool printVar);

/***
 * @brief Get size of terminal window
 * @param width reference to integer variable for width
 * @param height reference to integer variable for height
 * @return `0`-successful; `-1`-error
*/
int getTermSize(int &width, int &height) {
    struct winsize winDim;
    if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&winDim)==-1) {
        return -1;
    }
    width   = winDim.ws_col;
    height  = winDim.ws_row;
    return 0;
}

std::string getDate() {
    time_t currDate = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return ctime(&currDate);
}

/**
 * @brief method to allow ANSI printing string with newlines
 * @param toPrint string with newlines to print (doesn't need to have newlines)
 * @param xPos terminal x position to print on
 * @param yPos terminal y position to print on
 * @param flushEnd whether to flush `std::cout`
*/
void ANSI_mvprint(
    int posX,
    int posY,
    std::string printText,
    bool flushEnd = true
) {
    std::string ansiCode = "\x1B[";

    size_t tPos=0, ePos=0;
    int rowCount=0;
    while(printText.find('\n', tPos)!=std::string::npos) {
        ePos = printText.find('\n', tPos);
        std::cout<<ansiCode<<posY+rowCount<<";"<<posX<<"H"<<printText.substr(tPos, ePos-tPos);
        tPos = ePos+1;
        rowCount++;
    }
    std::cout<<ansiCode<<posY+rowCount<<";"<<posX<<"H"<<printText.substr(tPos, printText.length());

    // std::cout<<ansiCode<<posY<<";"<<posX<<"H"<<printText;
    if(flushEnd) std::cout.flush();
}

void baseAnsiPrint(
    std::vector<std::string> textVec,
    int posX,
    int posY,
    bool flushEnd = true,
    bool clearScr = false,
    std::string end = "\n",
    bool initClear = true
) {
    int terminalDim[2];
    static int oldTermDim[2];
    static bool funcInit = false;
    std::string ansiCode = "\x1B[";

    getTermSize(terminalDim[0], terminalDim[1]);

    if(!funcInit) {
        oldTermDim[0] = terminalDim[0];
        oldTermDim[1] = terminalDim[1];
        if(initClear) {
            std::cout << ansiCode+"2J";
            funcInit = true;
        }
    }
    if(terminalDim[0]!=oldTermDim[0] || terminalDim[1]!=oldTermDim[1]) {
        std::cout << ansiCode+"2J";
        oldTermDim[0] = terminalDim[0];
        oldTermDim[1] = terminalDim[1];
    }

    if(clearScr) std::cout << ansiCode+"2J";
    for(int i=0; i<static_cast<int>(textVec.size()); i++) {
        std::cout << ansiCode+std::to_string(posY+i)+";"+std::to_string(posX)+"H"+textVec[i];
    }
    std::cout << end;
    if(flushEnd) std::cout.flush();
}

/**
 * @brief ANSI print with terminal cursor coordinates
 * @param text text/paragraph to print on terminal
 * @param posX x-axis cursor position
 * @param posY y-axis cursor position
 * @param flushEnd whether to flush `std::cout` after printing
 * @param clearScr whether to clear screen before printing to terminal
 * @param textDelim delimiter std::string to indicate where to split `text` into separate lines/rows
 * @param end std::string to print at the end
*/
void ansiPrint(
    std::string text,
    int posX = 0,
    int posY = 0,
    bool flushEnd = true,
    bool clearScr = false,
    std::string textDelim = "\n",
    std::string end = "\n"
) {
    int printPos[2] = {0, 0};
    int maxRowLen = 0;

    /// @brief vector to hold each line of `text`
    std::vector<std::string> lines = splitString(text,textDelim,false);
    
    baseAnsiPrint(lines,posX,posY,flushEnd,clearScr,end);
}
/**
 * @brief ANSI print with terminal side percentage placement
 * @param text text/paragraph to print on terminal
 * @param xAlign percentage of terminal x-axis side length to place text
 * @param yAlign percentage of terminal y-axis side length to place text
 * @param flushEnd whether to flush `std::cout` after printing
 * @param clearScr whether to clear screen before printing to terminal
 * @param textDelim delimiter std::string to indicate where to split `text` into separate lines/rows
 * @param end std::string to print at the end
*/
void ansiPrint(
    std::string text,
    float scalX = 0,
    float scalY = 0,
    bool flushEnd = true,
    bool clearScr = false,
    std::string textDelim = "\n",
    std::string end = "\n",
    bool initClear = true
) {
    int terminalDim[2] = {0, 0};
    int printPos[2] = {0, 0};
    int textDim[2] = {0, 0};

    /// @brief vector to hold each line of `text`
    std::vector<std::string> lines = splitString(text,textDelim,false);
    getTermSize(terminalDim[0], terminalDim[1]);
    textDim[0] = lines[stringOfVector(lines,1)].length();
    textDim[1] = static_cast<int>(lines.size());
    printPos[0] = static_cast<int>(round(static_cast<float>(terminalDim[0])*scalX));
    printPos[1] = static_cast<int>(round(static_cast<float>(terminalDim[1])*scalY));
    if(printPos[0]+textDim[0]>terminalDim[0]) printPos[0]+=(terminalDim[0]-(printPos[0]+textDim[0]));
    if(printPos[1]+textDim[1]>terminalDim[1]) printPos[1]+=(terminalDim[1]-(printPos[1]+textDim[1]));
    
    baseAnsiPrint(lines,printPos[0],printPos[1],flushEnd,clearScr,end,initClear);
}

/**
 * @brief ANSI print with side alignment
 * @param text text/paragraph to print on terminal
 * @param xAlign x-axis side alignment:
 * options: {`"left"`, `"right"`}
 * @param yAlign y-axis side alignment:
 * options: {`"top"`, `"bottom"`}
 * @param flushEnd whether to flush `std::cout` after printing
 * @param clearScr whether to clear screen before printing to terminal
 * @param textDelim delimiter std::string to indicate where to split `text` into separate lines/rows
 * @param end std::string to print at the end
*/
void ansiPrint(
    std::string text,
    std::string xAlign,
    std::string yAlign,
    bool flushEnd = true,
    bool clearScr = false,
    std::string textDelim = "\n",
    std::string end = "\n"
) {
    float axisScal[2] = {0, 0};
    if(xAlign=="left") axisScal[0] = 0;
    else if(xAlign=="right") axisScal[0] = 1;
    else {
        std::cout << "ERROR: ansiPrint(): wrong xAlign parameter input. Exiting.."<<std::endl;
        return;
    }
    if(yAlign=="top") axisScal[1] = 0;
    else if(yAlign=="bottom") axisScal[1] = 1;
    else {
        std::cout << "ERROR: ansiPrint(): wrong yAlign parameter input. Exiting.."<<std::endl;
        return;
    }
    ansiPrint(text,axisScal[0],axisScal[1],flushEnd,clearScr,textDelim, end);
}

/// @brief Convert radians to degrees
/// @param radians radians to convert. type: float()
/// @return return degrees. type: float()
float toDegrees(float radians) { return (radians*180)/M_PI; }
/// @brief Convert degrees to radians
/// @param degrees degrees to convert. type: float()
/// @return return radians. type: float()
float toRadians(float degrees) { return float(degrees*M_PI)/180; }

/**
 * @brief Get the absolute path of the current directory
 * @param inclEndSlash whether to include `/` at the end of return path string
 * @return std::string of path:
*/
std::string getFileCWD(bool inclEndSlash=true) {
    char cwd[PATH_MAX];
    std::string returStr = cwd;
    if(getcwd(cwd, sizeof(cwd)) != NULL) {
        if(inclEndSlash) return returStr+"/";
        else return returStr;
    }
    else {
        std::cout << "getcwd() error." << std::endl;
        return "";
    }
}

/**
 * @brief check if `filename` file exists
 * @param filename name(with or without path) of file to check
 * @return boolean for if it exists
*/
bool isFile(std::string filename) {
    struct stat sb;
    if(stat(filename.c_str(), &sb)==0 && !(sb.st_mode & S_IFDIR)) {
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief check if `dirname` directive exists
 * @param dirname name of directive (with or without path) to check
 * @return boolean for if it exists
*/
bool isDir(std::string dirname) {
    struct stat sb;
    if(stat(dirname.c_str(), &sb)==0) {
        return true;
    }
    else {
        return false;
    }
}


/// @brief Solve distance value between two coordinates in a 3D spacec
/// @param p1 float()[3]: point 1 coordinate {x, y, z}
/// @param p2 float()[3]: point 2 coordinate {x, y, z}
/// @return float() type of absolute straight distance
float get3dDistance(float p1[3], float p2[3]) { return sqrt(pow(p2[0]-p1[0],2) + pow(p2[1]-p1[1],2) + pow(p2[2]-p1[2],2)); }


/// @brief Solve whether a number is positive or negative (same as int(var/abs(var)))
/// @param var variable to check
/// @return 1 if var>0; -1 if var<0; 0 if var==0
int PoN(float var) {
	if(var>0) return 1;
	else if(var<0) return -1;
	else {
		std::cout << "PoN(): \"" << var << "\" is not a number\n";
		return 0;
	}
}


/// @brief Find index to closest value in arr
/// @param arr std::vector<float>: the hay in which to find the needle in
/// @param needle value to find the closest to in the array
/// @param printVar whether to print results
/// @return index of result (closest value)
int getClosestValIdx(std::vector<float> arr, float needle, bool printVar=false) {
    std::vector<float> diffVec;
    std::vector<int> foundIdx;
    float minVar;

    for(int i=0; i<arr.size(); i++) { diffVec.push_back(abs(arr.at(i)-needle)); }
    minVar = *min_element(diffVec.begin(), diffVec.end());
    for(int i=0; i<diffVec.size(); i++) { if(diffVec.at(i)==minVar) foundIdx.push_back(i); }
    if(foundIdx.size()==0) return -1;
    return foundIdx.at(0);
}
/// @brief Find index to closest value in arr
/// @param arr arr[]: the 
/// @param arrLen size of the array
/// @param needle value to find the closest to in the array
/// @param printVar whether to print results
/// @return index of result (closest value)
int getClosestValIdx(float arr[], int arrLen, float needle, bool printVar=false) {
    std::vector<float> diffVec;
    std::vector<int> foundIdx;
    float minVar;

    for(int i=0; i<arrLen; i++) { diffVec.push_back(abs(arr[i]-needle)); }
    minVar = *min_element(diffVec.begin(), diffVec.end());
    for(int i=0; i<diffVec.size(); i++) { if(diffVec.at(i)==minVar) foundIdx.push_back(i); }
    if(foundIdx.size()==0) return -1;
    return foundIdx.at(0);
}


void splitString(
    std::string line,
    std::string delimiter,
    float returnArr[],
    int numVar=4,
    bool printVar=false
) {
    if(printVar) std::cout << "--- \"" << line << "\"\n";
    size_t pos = 0;
    for(int i=0; i<numVar; i++) {
        if(i<(numVar-1)) pos = line.find(delimiter);
        if(printVar) std::cout << "- pos:" << pos << " :" << line.substr(0, pos) << "\n";
        returnArr[i] = stof(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
    }
    if(printVar) std::cout << "---";
}

std::vector<std::string> splitString(
    std::string line,
    std::string delimiter,
    bool printVar=false
) {
    if(printVar) std::cout << "--- \"" << line << "\"\n";
    size_t pos = 0;
    std::vector<std::string> resultStrings;
    if(line.find(delimiter)==std::string::npos) {
        resultStrings.push_back(line);
        std::cout << "error: splitString: no delimiter \"" << delimiter << "\" found. Returning empty vector\n";
        return resultStrings;
    }
    while(true) {
        pos = line.find(delimiter);
        if(printVar) std::cout << "- pos:" << pos << " :" << line.substr(0, pos) << "\n";
        resultStrings.push_back(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
        if(line.find(delimiter)==std::string::npos) {
            resultStrings.push_back(line);
            break;
        }
    }
    if(printVar) std::cout << "---";
    return resultStrings;
}
void splitString(
    std::string line,
    std::string delimiter,
    std::vector<std::string> &returnVec,
    bool printVar=false
) {
    returnVec.clear();
    returnVec = splitString(line,delimiter,printVar);
}
void splitString(
    std::string line,
    std::string delimiter,
    std::vector<std::string> *returnPtr,
    bool printVar=false
) {
    returnPtr->clear();
    (*returnPtr) = splitString(line,delimiter,printVar);
}


/// @brief replace every `{toReplace}` std::string in `{text}` with `{replaceTo}`
/// @param text std::string to replace parts of
/// @param toReplace old std::string that is to be removed/replaced
/// @param replaceTo new std::string that will be inserted
/// @return original `{text}` std::string but with `{toReplace}` replaced
std::string replaceSubstr(std::string text, std::string toReplace, std::string replaceTo) {
	size_t count=0;
    size_t pos = text.find(toReplace);
    int replaceLen = toReplace.length();
    
	while(pos!=std::string::npos) {
        text.replace(pos, replaceLen, replaceTo);
        count+=pos;
        pos = text.find(toReplace, pos+1);
    }
    return text;
}
void replaceSubstr(std::string* text, std::string toReplace, std::string replaceTo) {
	size_t count=0;
    size_t pos = text->find(toReplace);
    int replaceLen = toReplace.length();
    
	while(pos!=std::string::npos) {
        text->replace(pos, replaceLen, replaceTo);
        count+=pos;
        pos = text->find(toReplace, pos+1);
    }
}


/// @brief input float value into a char array with set decimal precision, width and start position in said char array
/// @param inpVal float value to insert into char array
/// @param idx_start index to start pos where float value is to be inserted; total occupied length is same as width param
/// @param toSend_arr pointer to char array
/// @param width width of the total char std::string including decimal dot and minus sign
/// @param precision decimal accuracy number: how many numbers of precision after decimal place
/// @param leftAlign whether to align the float to the left in the array
void fillCharArray(
    float inpVal,
    int idx_start,
    char *toSend_arr,
    int width=0,
    int precision=0,
    bool leftAlign=false
) {
	char fullTemp[width+1];
    
    if(leftAlign) snprintf(fullTemp, width+1, "%-*.*f", width, precision, inpVal);
    else snprintf(fullTemp, width+1, "%*.*f", width, precision, inpVal);
    
    for(int i=0; i<strlen(fullTemp); i++) {
    	toSend_arr[idx_start+i]=fullTemp[i];
    }
}

/// @brief Convert float value to std::string with `width` 
/// number of chars where everything not the number is filled with space
/// @param inpVal decimal number to convert to string
/// @param width number of char for the string
/// @param precision number of decimals for the decimal number in string
/// @param leftAlign whether for the number to align left side
/// @return std::string of number
std::string formattedFloat_c(
    float inpVal,
    int width=0,
    int precision=0,
    bool leftAlign=false
) {
    char temp[width+1];
    if(leftAlign) sprintf(temp, "%-*.*f", width, precision, inpVal);
    else sprintf(temp, "%*.*f", width, precision, inpVal);

    std::string tempStr(temp);
    return tempStr;
}
void formattedFloat_c(
    float inpVal,
    std::string *strVarPtr,
    int width=0,
    int precision=0,
    bool leftAlign=false
) {
    *strVarPtr = formattedFloat_c(inpVal, width, precision, leftAlign);
}

template<class T>
std::string FormatWithSymbol(T value, std::string formatSymbol=" ")
{
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << value;
    return replaceSubstr(ss.str(), ",", formatSymbol);
}
/// @brief Convert decimal number to std::string with set preicision
/// @tparam T 
/// @param value decimal number to convert/type-cast
/// @param varPrecision number of decimals
/// @return std::string of `value` with `varPrecision` number of decimal numbers
template<class T>
std::string formatNumber(T value, int varPrecision=2) {
	std::stringstream tempStream;
    tempStream << std::fixed << std::setprecision(varPrecision) << value;
    return tempStream.str();
}
/// @brief Convert decimal number to std::string with set decimal numbers and minimum total width
/// @tparam T 
/// @param value decimal number to convert
/// @param strWidth minimum width for the string
/// @param varPrecision decimal precision
/// @param align whether to align value with left or right side: {`"left"`, `"right"`},
/// @return returns formatted string
template<class T>
std::string formatNumber(
    T value,
    int strWidth,
    int varPrecision,
    std::string align="right"
) {
    std::stringstream outStream;
    outStream << std::fixed;
    if(align=="left") outStream<<std::left;
    else if(align=="right") outStream<<std::right;
    outStream << std::setw(strWidth) << std::setprecision(varPrecision) << value;
    return outStream.str();
}


/// @brief create a progress bar on terminal
/// @param progress current progress made
/// @param total_val total progress which reesembles 100&
/// @param printBar whether to print the progress bar
/// @param progFin whether the progress is finished (used to get total duration)
/// @param interval the time interval between each progressBar update
/// @param symbolIndex what symbol to use {"■", "⬛", "▉", "▉", "█"}
/// @return percent of the progress made
float progressBar(
    float progress,
    float total_val,
    bool printBar   = true,
    bool progFin    = false,
    float interval  = 0.01,
    int symbolIndex = 3
) {
    static int symbIdx = 2;
    symbIdx = symbolIndex;
    static std::string symb[] = {"■", "⬛", "▉", "▉", "█"};
    static char intr[] = {'|', '/', '-', '\\'};
    static int prev_intrCount = 0;
    static bool func_initd = false;
    static bool func_ended = true;
    static int total_progLen;
    static float speed, percent = 0;

    static float prev_progress = 0;
    static auto prevTime = std::chrono::steady_clock::now();
    static auto start_time = std::chrono::system_clock::now();
    static auto abs_startTime = std::chrono::system_clock::now();
    static float progressDiff = 0;

    static float filterVal = 0.1;

    static int total_val_sanityCheck = 0;


    if(!func_initd) total_val_sanityCheck = int(total_val);

    percent = progress/total_val*100;

    auto currTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(currTime-prevTime);

    if(!progFin && float(elapsed.count()/float(1'000'000))<interval) return percent;

    speed = filterVal*(progress-prev_progress)/(elapsed.count()/float(1'000'000)) + (1.0-filterVal)*speed;
    progressDiff = float(0.5)*(progress-prev_progress)+float(0.5)*progressDiff;
    prevTime = currTime;
    prev_progress = progress;

    if(!func_initd) {
        abs_startTime = std::chrono::system_clock::now();
        time_t tStart_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        char* startTime_text = ctime(&tStart_time);
        if (startTime_text[strlen(startTime_text)-1] == '\n') startTime_text[strlen(startTime_text)-1] = '\0';
        std::cout << "Start time: [" << startTime_text << "]" << std::endl << std::endl;
        total_progLen = FormatWithSymbol(int(total_val), "'").length();
        func_initd = true;
    }
    if(int(total_val) != total_val_sanityCheck) {
        func_initd = false;
    }


    std::string prog_formatted = FormatWithSymbol(int(progress), "'");
    std::string emptSpac(total_progLen-prog_formatted.length(), ' ');
    std::string totalStr = " progress: "+emptSpac+prog_formatted+": ";

    std::string percent_formatted = formatNumber(percent,2);
    std::string emptSpac2(6-percent_formatted.length(), ' ');
    totalStr += emptSpac2+percent_formatted+"% ";

    std::string progBars="";
    for(int i=0; i<=int(floor(percent)); i++) {
        progBars+=symb[symbIdx];
    }

    std::string progressStr;
    if(int(floor(percent))<100) progressStr = progBars+intr[prev_intrCount];
    else progressStr = progBars;
    prev_intrCount+=1;
    if(prev_intrCount>=4) prev_intrCount=0;

    std::string emptSpac3(100-int(floor(percent)), ' ');
    totalStr += "|"+progressStr+emptSpac3+"|: ";

    std::string speed_formatted = formatNumber(speed, 1);
    std::string emptSpac4(6-speed_formatted.length(), ' ');
    totalStr += emptSpac4+speed_formatted+"pt/s ";

    if(printBar) printf(" %s\r", totalStr.c_str());

    if(progFin) {
        auto tEnd_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        char* endTime_text = ctime(&tEnd_time);

        if (endTime_text[strlen(endTime_text)-1] == '\n') endTime_text[strlen(endTime_text)-1] = '\0';
        std::cout << "\n\nEnd time  : [" << endTime_text << "]\n";

        std::chrono::duration<double> elapsedTime = std::chrono::system_clock::now()-abs_startTime;

        const auto hrs = std::chrono::duration_cast<std::chrono::hours>(elapsedTime);
        const auto mins = std::chrono::duration_cast<std::chrono::minutes>(elapsedTime - hrs);
        const auto secs = std::chrono::duration_cast<std::chrono::seconds>(elapsedTime - hrs - mins);
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime - hrs - mins - secs);
        std::cout << "System inactive for [" << hrs.count() <<
        ":" << mins.count() <<
        ":" << secs.count() <<
        "." << ms.count() << "]"<< std::endl;
        // std::cout << "elapsed time: [" << elapsedTime.count() << "s]\n";
    }

    return percent;
}


#endif
