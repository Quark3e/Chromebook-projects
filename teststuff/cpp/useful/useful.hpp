
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

#include <cstring>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <linux/limits.h>

using namespace std;


// #include "createTable.hpp"


static string dummyRef_stringOfVector_resultStrRef;
/**
 * @brief find specific string from `std::vector<std::string>` parameter
 * @param stringVec `std::vector<std::string>` find find specific string from
 * @param id id for what to look for:
 * `0` - find shortest string;
 * `1` - find longest string;
 * @return index of the string in the vector
*/
int stringOfVector(
    vector<string> stringVec,
    int id,
    string& resultStrRef = dummyRef_stringOfVector_resultStrRef
) {
    int idx;
    vector<string>::iterator leWord;

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
        cout << "ERROR: stringOfVector: wrong id selected. Exiting.." << endl;
        return -1;
    }
    resultStrRef = *leWord;
    return distance(stringVec.begin(), leWord);
}

void splitString(string line, string delimiter, float returnArr[], int numVar, bool printVar);

/**
 * @brief Split and return `line` string by each `delimiter`
 * @param line string to split.
 * @param delimiter string for where to split the string `line` by.
 * @param printVar whether to print each delimiter and substring
*/
vector<string> splitString(string line, string delimiter, bool printVar);
/**
 * @brief Split `line` string by each `delimiter` and pass the result to reference parameter `returnVec`
 * @param line string to split.
 * @param delimiter string for where to split the string `line` by.
 * @param returnVec reference to a `vector<string>` for which to pass result to
 * @param printVar whether to print each delimiter and substring
*/
void splitString(string line, string delimiter, vector<string> &returnVec, bool printVar);
/**
 * @brief Split `line` string by each `delimiter` and pass the result to a pointer parameter `returnPtr`
 * @param line string to split.
 * @param delimiter string for where to split the string `line` by.
 * @param returnPtr pointer to a `vector<string>` for which to pass result to
 * @param printVar whether to print each delimiter and substring
*/
void splitString(string line, string delimiter, vector<string> *returnPtr, bool printVar);

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

string getDate() {
    time_t currDate = chrono::system_clock::to_time_t(chrono::system_clock::now());
    return ctime(&currDate);
}


void baseAnsiPrint(
    vector<string> textVec,
    int posX,
    int posY,
    bool flushEnd = true,
    bool clearScr = false,
    string end = "\n"
) {
    string ansiCode = "\x1B[";

    if(clearScr) cout << ansiCode+"2J";
    for(int i=0; i<static_cast<int>(textVec.size()); i++) {
        cout << ansiCode+to_string(posY+i)+";"+to_string(posX)+"H"+textVec[i];
    }
    cout << end;
    if(flushEnd) cout.flush();
}

/**
 * @brief ANSI print with terminal side percentage placement
 * @param text text/paragraph to print on terminal
 * @param posX x-axis cursor position
 * @param posY y-axis cursor position
 * @param flushEnd whether to flush `std::cout` after printing
 * @param clearScr whether to clear screen before printing to terminal
 * @param textDelim delimiter string to indicate where to split `text` into separate lines/rows
 * @param end string to print at the end
*/
void ansiPrint(
    string text,
    int posX = 0,
    int posY = 0,
    bool flushEnd = true,
    bool clearScr = false,
    string textDelim = "\n",
    string endSymb = "\n"
) {
    static bool funcInit = false;
    int printPos[2] = {0, 0};
    int maxRowLen = 0;

    /// @brief vector to hold each line of `text`
    vector<string> lines = splitString(text,textDelim,false);
    
}
/**
 * @brief ANSI print with terminal side percentage placement
 * @param text text/paragraph to print on terminal
 * @param xAlign percentage of terminal x-axis side length to place text
 * @param yAlign percentage of terminal y-axis side length to place text
 * @param flushEnd whether to flush `std::cout` after printing
 * @param clearScr whether to clear screen before printing to terminal
 * @param textDelim delimiter string to indicate where to split `text` into separate lines/rows
 * @param end string to print at the end
*/
void ansiPrint(
    string text,
    float scalX = 0,
    float scalY = 0,
    bool flushEnd = true,
    bool clearScr = false,
    string textDelim = "\n",
    string endSymb = "\n"
) {
    static bool funcInit = false;
    int terminalDim[2] = {0, 0};
    int printPos[2] = {0, 0};
    int textDim[2] = {0, 0};

    /// @brief vector to hold each line of `text`
    vector<string> lines = splitString(text,textDelim,false);
    getTermSize(terminalDim[0], terminalDim[1]);
    textDim[0] = lines[stringOfVector(lines,1)].length();
    textDim[1] = static_cast<int>(lines.size());
    printPos[0] = static_cast<int>(round(static_cast<float>(terminalDim[0])*scalX));
    printPos[1] = static_cast<int>(round(static_cast<float>(terminalDim[1])*scalY));
    if(printPos[0]+textDim[0]>terminalDim[0]) printPos[0]+=(terminalDim[0]-(printPos[0]+textDim[0]));
    if(printPos[1]+textDim[1]>terminalDim[1]) printPos[1]+=(terminalDim[1]-(printPos[1]+textDim[1]));
    
    baseAnsiPrint(lines,printPos[0],printPos[1],flushEnd,clearScr,endSymb);
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
 * @param textDelim delimiter string to indicate where to split `text` into separate lines/rows
 * @param end string to print at the end
*/
void ansiPrint(
    string text,
    string xAlign,
    string yAlign,
    bool flushEnd = true,
    bool clearScr = false,
    string textDelim = "\n",
    string end = "\n"
) {
    float axisScal[2] = {0, 0};
    if(xAlign=="left") axisScal[0] = 0;
    else if(xAlign=="right") axisScal[0] = 1;
    else {
        cout << "ERROR: ansiPrint(): wrong xAlign parameter input. Exiting.."<<endl;
        return;
    }
    if(yAlign=="top") axisScal[1] = 0;
    else if(yAlign=="bottom") axisScal[1] = 1;
    else {
        cout << "ERROR: ansiPrint(): wrong yAlign parameter input. Exiting.."<<endl;
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
 * @return string of path:
*/
string getFileCWD(bool inclEndSlash=true) {
    char cwd[PATH_MAX];
    string returStr = cwd;
    if(getcwd(cwd, sizeof(cwd)) != NULL) {
        if(inclEndSlash) return returStr+"/";
        else return returStr;
    }
    else {
        cout << "getcwd() error." << endl;
        return "";
    }
}

/**
 * @brief check if `filename` file exists
 * @param filename name(with or without path) of file to check
 * @return boolean for if it exists
*/
bool isFile(string filename) {
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
bool isDir(string dirname) {
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
		cout << "PoN(): \"" << var << "\" is not a number\n";
		return 0;
	}
}


/// @brief Find index to closest value in arr
/// @param arr vector<float>: the hay in which to find the needle in
/// @param needle value to find the closest to in the array
/// @param printVar whether to print results
/// @return index of result (closest value)
int getClosestValIdx(vector<float> arr, float needle, bool printVar=false) {
    vector<float> diffVec;
    vector<int> foundIdx;
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
    vector<float> diffVec;
    vector<int> foundIdx;
    float minVar;

    for(int i=0; i<arrLen; i++) { diffVec.push_back(abs(arr[i]-needle)); }
    minVar = *min_element(diffVec.begin(), diffVec.end());
    for(int i=0; i<diffVec.size(); i++) { if(diffVec.at(i)==minVar) foundIdx.push_back(i); }
    if(foundIdx.size()==0) return -1;
    return foundIdx.at(0);
}


void splitString(
    string line,
    string delimiter,
    float returnArr[],
    int numVar=4,
    bool printVar=false
) {
    if(printVar) cout << "--- \"" << line << "\"\n";
    size_t pos = 0;
    for(int i=0; i<numVar; i++) {
        if(i<(numVar-1)) pos = line.find(delimiter);
        if(printVar) cout << "- pos:" << pos << " :" << line.substr(0, pos) << "\n";
        returnArr[i] = stof(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
    }
    if(printVar) cout << "---";
}

vector<string> splitString(
    string line,
    string delimiter,
    bool printVar=false
) {
    if(printVar) cout << "--- \"" << line << "\"\n";
    size_t pos = 0;
    vector<string> resultStrings;
    if(line.find(delimiter)==string::npos) {
        resultStrings.push_back(line);
        cout << "error: splitString: no delimiter \"" << delimiter << "\" found. Returning empty vector\n";
        return resultStrings;
    }
    while(true) {
        pos = line.find(delimiter);
        if(printVar) cout << "- pos:" << pos << " :" << line.substr(0, pos) << "\n";
        resultStrings.push_back(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
        if(line.find(delimiter)==string::npos) {
            resultStrings.push_back(line);
            break;
        }
    }
    if(printVar) cout << "---";
    return resultStrings;
}
void splitString(
    string line,
    string delimiter,
    vector<string> &returnVec,
    bool printVar=false
) {
    returnVec.clear();
    returnVec = splitString(line,delimiter,printVar);
}
void splitString(
    string line,
    string delimiter,
    vector<string> *returnPtr,
    bool printVar=false
) {
    returnPtr->clear();
    (*returnPtr) = splitString(line,delimiter,printVar);
}


/// @brief replace every `{toReplace}` string in `{text}` with `{replaceTo}`
/// @param text string to replace parts of
/// @param toReplace old string that is to be removed/replaced
/// @param replaceTo new string that will be inserted
/// @return original `{text}` string but with `{toReplace}` replaced
string replaceSubstr(string text, string toReplace, string replaceTo) {
	size_t count=0;
    size_t pos = text.find(toReplace);
    int replaceLen = toReplace.length();
    
	while(pos!=string::npos) {
        text.replace(pos, replaceLen, replaceTo);
        count+=pos;
        pos = text.find(toReplace, pos+1);
    }
    return text;
}
void replaceSubstr(string* text, string toReplace, string replaceTo) {
	size_t count=0;
    size_t pos = text->find(toReplace);
    int replaceLen = toReplace.length();
    
	while(pos!=string::npos) {
        text->replace(pos, replaceLen, replaceTo);
        count+=pos;
        pos = text->find(toReplace, pos+1);
    }
}


/// @brief input float value into a char array with set decimal precision, width and start position in said char array
/// @param inpVal float value to insert into char array
/// @param idx_start index to start pos where float value is to be inserted; total occupied length is same as width param
/// @param toSend_arr pointer to char array
/// @param width width of the total char string including decimal dot and minus sign
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

/// @brief Convert float value to string with `width` 
/// number of chars where everything not the number is filled with space
/// @param inpVal decimal number to convert to string
/// @param width number of char for the string
/// @param precision number of decimals for the decimal number in string
/// @param leftAlign whether for the number to align left side
/// @return string of number
string formattedFloat_c(
    float inpVal,
    int width=0,
    int precision=0,
    bool leftAlign=false
) {
    char temp[width+1];
    if(leftAlign) sprintf(temp, "%-*.*f", width, precision, inpVal);
    else sprintf(temp, "%*.*f", width, precision, inpVal);

    string tempStr(temp);
    return tempStr;
}
void formattedFloat_c(
    float inpVal,
    string *strVarPtr,
    int width=0,
    int precision=0,
    bool leftAlign=false
) {
    *strVarPtr = formattedFloat_c(inpVal, width, precision, leftAlign);
}

template<class T>
string FormatWithSymbol(T value, string formatSymbol=" ")
{
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << value;
    return replaceSubstr(ss.str(), ",", formatSymbol);
}
/// @brief Convert decimal number to string with set preicision
/// @tparam T 
/// @param value decimal number to convert/type-cast
/// @param varPrecision number of decimals
/// @return string of `value` with `varPrecision` number of decimal numbers
template<class T>
string formatNumber(T value, int varPrecision=2) {
	stringstream tempStream;
    tempStream << fixed << setprecision(varPrecision) << value;
    return tempStream.str();
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
    static string symb[] = {"■", "⬛", "▉", "▉", "█"};
    static char intr[] = {'|', '/', '-', '\\'};
    static int prev_intrCount = 0;
    static bool func_initd = false;
    static bool func_ended = true;
    static int total_progLen;
    static float speed, percent = 0;

    static float prev_progress = 0;
    static auto prevTime = chrono::steady_clock::now();
    static auto start_time = chrono::system_clock::now();
    static auto abs_startTime = chrono::system_clock::now();
    static float progressDiff = 0;

    static float filterVal = 0.1;

    static int total_val_sanityCheck = 0;


    if(!func_initd) total_val_sanityCheck = int(total_val);

    percent = progress/total_val*100;

    auto currTime = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::microseconds>(currTime-prevTime);

    if(!progFin && float(elapsed.count()/float(1'000'000))<interval) return percent;

    speed = filterVal*(progress-prev_progress)/(elapsed.count()/float(1'000'000)) + (1.0-filterVal)*speed;
    progressDiff = float(0.5)*(progress-prev_progress)+float(0.5)*progressDiff;
    prevTime = currTime;
    prev_progress = progress;

    if(!func_initd) {
        abs_startTime = chrono::system_clock::now();
        time_t tStart_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
        char* startTime_text = ctime(&tStart_time);
        if (startTime_text[strlen(startTime_text)-1] == '\n') startTime_text[strlen(startTime_text)-1] = '\0';
        cout << "Start time: [" << startTime_text << "]" << endl << endl;
        total_progLen = FormatWithSymbol(int(total_val), "'").length();
        func_initd = true;
    }
    if(int(total_val) != total_val_sanityCheck) {
        func_initd = false;
    }


    string prog_formatted = FormatWithSymbol(int(progress), "'");
    string emptSpac(total_progLen-prog_formatted.length(), ' ');
    string totalStr = " progress: "+emptSpac+prog_formatted+": ";

    string percent_formatted = formatNumber(percent,2);
    string emptSpac2(6-percent_formatted.length(), ' ');
    totalStr += emptSpac2+percent_formatted+"% ";

    string progBars="";
    for(int i=0; i<=int(floor(percent)); i++) {
        progBars+=symb[symbIdx];
    }

    string progressStr;
    if(int(floor(percent))<100) progressStr = progBars+intr[prev_intrCount];
    else progressStr = progBars;
    prev_intrCount+=1;
    if(prev_intrCount>=4) prev_intrCount=0;

    string emptSpac3(100-int(floor(percent)), ' ');
    totalStr += "|"+progressStr+emptSpac3+"|: ";

    string speed_formatted = formatNumber(speed, 1);
    string emptSpac4(6-speed_formatted.length(), ' ');
    totalStr += emptSpac4+speed_formatted+"pt/s ";

    if(printBar) printf(" %s\r", totalStr.c_str());

    if(progFin) {
        auto tEnd_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
        char* endTime_text = ctime(&tEnd_time);

        if (endTime_text[strlen(endTime_text)-1] == '\n') endTime_text[strlen(endTime_text)-1] = '\0';
        cout << "\n\nEnd time  : [" << endTime_text << "]\n";

        chrono::duration<double> elapsedTime = chrono::system_clock::now()-abs_startTime;

        const auto hrs = chrono::duration_cast<chrono::hours>(elapsedTime);
        const auto mins = chrono::duration_cast<chrono::minutes>(elapsedTime - hrs);
        const auto secs = chrono::duration_cast<chrono::seconds>(elapsedTime - hrs - mins);
        const auto ms = chrono::duration_cast<chrono::milliseconds>(elapsedTime - hrs - mins - secs);
        cout << "System inactive for [" << hrs.count() <<
        ":" << mins.count() <<
        ":" << secs.count() <<
        "." << ms.count() << "]"<< endl;
        // cout << "elapsed time: [" << elapsedTime.count() << "s]\n";
    }

    return percent;
}


#endif