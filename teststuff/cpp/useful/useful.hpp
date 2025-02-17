
#pragma once
#ifndef HPP_USEFUL
#define HPP_USEFUL

#ifndef M_PI
#define M_PI  3.1415926535
#endif

#include <bitset>
#include <math.h>
#include <cmath>
#include <string>
#include <vector>
#include <array>
#include <initializer_list>
#include <iostream>
// #include <bits/stdc++.h>
#include <chrono>
#include <ctime>

#include <algorithm>

#include <cstring>
#include <stdlib.h>

#include <cassert>

#include <stdio.h>

#include <thread>
#include <mutex>

#include <sstream>
#include <iostream>
#include <iomanip>


#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#else

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/limits.h>
#include <unistd.h>

#endif


template<class _contType>
inline bool decimalSame(_contType _var0, _contType _var1, size_t _contSize, size_t _precision=6) {
    bool matched = true;
    for(size_t i=0; i<_contSize; i++) {
        if(roundf(_var0[i]*pow(10, _precision))/pow(10, _precision) != roundf(_var1[i]*pow(10, _precision))/pow(10, _precision)) {
            matched = false;
            break;
        }
    }
    return matched;
}

#ifndef _VAR__pos2d
#define _VAR__pos2d

template<typename _varType>
struct pos2d {
    _varType x;
    _varType y;
    pos2d() {}
    // pos2d(_varType *_arrPtr): x(_arrPtr[0]), y(_arrPtr[1]) {}
    pos2d(const pos2d &_copy) {
        this->x = _copy.x;
        this->y = _copy.y;
    }
    pos2d(_varType _x, _varType _y): x(_x), y(_y) {}
    _varType &operator[](size_t _i) {
        switch (_i) {
            case 0: return this->x;
            case 1: return this->y;
        }
        throw std::out_of_range("size_t index value is out of range.");
        return x;
    }
    _varType operator[](size_t _i) const {
        switch (_i) {
            case 0: return this->x;
            case 1: return this->y;
        }
        throw std::out_of_range("size_t index value is out of range.");
        return x;
    }

    pos2d &operator=(const pos2d<_varType>& m) {
        this->x = m.x;
        this->y = m.y;
        return *this;
    }
    bool operator<(pos2d const& m)  { return ((x-m.x)+(y-m.y))<0; }
    bool operator>(pos2d const& m)  { return ((x-m.x)+(y-m.y))>0; }
    int equalCompare_round_prec = 6;
    bool operator==(pos2d const& m) {
        return (
            (roundf(m.x*pow(10, equalCompare_round_prec))/pow(10, equalCompare_round_prec)==roundf(this->x*pow(10, equalCompare_round_prec))/pow(10, equalCompare_round_prec)) &&
            (roundf(m.y*pow(10, equalCompare_round_prec))/pow(10, equalCompare_round_prec)==roundf(this->y*pow(10, equalCompare_round_prec))/pow(10, equalCompare_round_prec))
        );
        // return (x==m.x && y==m.y);
    }
    bool operator!=(pos2d const& m) { return !(x==m.x && y==m.y); }

    /**
     * @brief Check whether a pos2d value is within the region given by min-max corners of a Bounding Box.
     * 
     * @param corner_min corner coordinate with minimum values.
     * @param corner_max corner coordinate with maximum values.
     * @param _includeBB whether to include the Bounding Box borner region (ex: `>=`) or not (`>`)
     * @return true is within region.
     * @return false is not within region.
     */
    bool inRegion(pos2d corner_min, pos2d corner_max, bool _includeBB=true) {
        bool inReg = false;
        if(_includeBB) {
            inReg = ((x>=corner_min.x && y>=corner_min.y) && (x<=corner_max.x && y<=corner_max.y));
        }
        else {
            inReg = ((x> corner_min.x && y> corner_min.y) && (x< corner_max.x && y< corner_max.y));
        }

        return inReg;
    }

    pos2d& operator+=(pos2d const& m) {
        x+=m.x;
        y+=m.y;
        return *this;
    }
    pos2d& operator-=(pos2d const& m) {
        x-=m.x;
        y-=m.y;
        return *this;
    }
    pos2d& operator*=(pos2d const& m) {
        x*=m.x;
        y*=m.y;
        return *this;
    }
    pos2d& operator/=(pos2d const& m) {
        x/=m.x;
        y/=m.y;
        return *this;
    }
    
    pos2d operator+(pos2d const& m) {
        return pos2d(x+m.x, y+m.y);
    }
    pos2d operator-(pos2d const& m) {
        return pos2d(x-m.x, y-m.y);
    }
    pos2d operator*(pos2d const& m) {
        return pos2d(x*m.x, y*m.y);
    }
    pos2d operator/(pos2d const& m) {
        return pos2d(x/m.x, y/m.y);
    }
    

    pos2d rounded(int decimals) {
        return pos2d(
            roundf(this->x*pow(10, decimals)) / pow(10, decimals),
            roundf(this->y*pow(10, decimals)) / pow(10, decimals)
        );
    }
    
    // template<class _twoSizeContainerType>
    // operator _twoSizeContainerType() {
    //     _twoSizeContainerType _var;
    //     _var[0] = x;
    //     _var[1] = y;
    //     return _var;
    // }


    int _printPrecision = 2;
    int _printWidth     = 0;
    friend auto operator<<(std::ostream &os, pos2d const& m) -> std::ostream& {
        os << std::setw(m._printWidth) << std::setprecision(m._printPrecision);
        os << "("<<m.x<<","<<m.y<<")";
        return os;
    }
};

#endif



// namespace USEFUL {


    /// @brief Convert decimal number to std::string with set decimal numbers and minimum total width
    /// @tparam T 
    /// @param value decimal number to convert
    /// @param strWidth minimum width for the string
    /// @param varPrecision decimal precision
    /// @param align whether to align value with left or right side: {`"left"`, `"right"`},
    /// @param numberFill whether to add 0's before the number to complete missing strWidth 
    /// @return returns formatted string
    template<class T>
    inline std::string formatNumber(
        T value,
        int strWidth,
        int varPrecision,
        std::string align="right",
        bool numberFill= false
    );

    
    inline size_t get_sumStrVec_len(std::vector<std::string> _vec) {
        size_t sz=0;
        for(size_t i=0; i<_vec.size(); i++) {
            sz+=_vec.at(i).size();
        }
        return sz;
    }


    /**
     * @brief Round the indexable numbers of a data type container to a specific decimal.
     * 
     * @tparam `_cont` data type of the container to round its indices/member variables.
     * @param toRound the container to round.
     * @param decimals number-of-decimals/precision to round to.
     * @param _contsSize innate size of the container. ex: `pos2d`/`ImVec2` has a size of two
     * @return _cont of `toRound` that's been rounded to `decimals` precision
     */
    template<typename _cont>
    inline _cont container_round(_cont toRound, int decimals, size_t _contsSize) {
        _cont tempCont;
        for(size_t i=0; i<_contsSize; i++) {
            tempCont[i] = round(toRound[i]*pow(10, decimals)) / pow(10, decimals);
        }
        return tempCont;
    }

    /// @brief return the sum of elements in an array of type `T`
    /// @tparam T 
    /// @param arr pointer to the array (array name) to find sum of
    /// @return the sum of all elements in `T` data type
    template<class T>
    inline double array_sum(T *arr, int arrSize) {
        double sum = 0;
        for(int i=0; i<arrSize; i++) sum += arr[i];
        return sum;
    }
    template<class T>
    inline double array_sum(T vec) {
        double sum = 0;
        for(size_t i=0; i<vec.size(); i++) sum += vec[i];
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

    inline bool array_bool_AND(bool *arrCheck, int arrSize) {
        for(int i=0; i<arrSize; i++) {
            if(!arrCheck[i]) return false;
        }
        return true;
    }
    inline bool array_bool_OR(bool *arrCheck, int arrSize) {
        for(int i=0; i<arrSize; i++) {
            if(arrCheck[i]) return true;
        }
        return false;
    }


    inline bool isNumber(const char* stringToCheck) {
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

    inline bool isNumber(std::string stringToCheck) {
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
    inline std::string cmdExec(const char* cmd, bool removeNewLine=true) {
        std::array<char, 128> buffer;
        std::string result;
#if _WIN32
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
#else
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
#endif

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
    inline int findOcc_char(std::string toCheck, char toFind) {
        std::string::difference_type n = std::count(toCheck.begin(), toCheck.end(), toFind);
        return static_cast<int>(n);
    }

    inline std::string removeFromString(std::string toModify, std::vector<char> toRemove) {
        if(toModify.size()==0) throw std::invalid_argument("toModify argument cannot be empty.");
        if(toRemove.size()==0) throw std::invalid_argument("toRemove arg cannot be empty.");
        std::string _retur;
        for(size_t i=0; i<toModify.size(); i++) {
            bool _next = false;
            for(size_t r=0; r<toRemove.size(); r++) {
                if(toModify.at(i)==toRemove.at(r)) {
                    _next = true;
                    break;
                }
            }
            if(_next) continue;
            _retur.push_back(toModify.at(i));
        }
        return _retur;
    }

    #include <cstdlib>
    #include <signal.h>
    /**
     * detect signal from callback
     * @param signum signal to catch on callback
     * @note to detect KeyboardInterrupt pass `signal(SIGINT, signal_callback_handler);` at the start of the program before the loop
    */
    inline void signal_callback_handler(int signum) {
    std::cout <<std::endl<< "Caught signal " << signum << std::endl;
    // Terminate program
    exit(signum);
    }
    // #include "createTable.hpp"

    /// @brief Find a substring inside of a larger string
    /// @param _toFind the string to locate/find
    /// @param _toSearch the string to search through to find `toFind`
    /// @param _startPos initial position to start the search from
    /// @return size_t to index position in _toSearch to the first letter of the found word's position. If no pos found then it'll return `std::string::npos`
    /// @note if `toFind` has more characters than `_toSearch` then this function will throw a `std::invalid_argument` exception.
    inline size_t findStringInString(std::string _toFind, std::string _toSearch, size_t _startPos=0) {
        static const std::string _info_name("findStringInString(std::string, std::string)");
        if(_toFind.size()==0)    throw std::invalid_argument(_info_name+" arg for _toFind cannot be empty.");
        if(_toSearch.size()==0)  throw std::invalid_argument(_info_name+" arg for _toSearch cannot be empty.");
        if(_toFind.size() > _toSearch.size()) throw std::invalid_argument(_info_name+" arg for _toFind cannot be larger than _toSearch.");

        size_t charsMatched = 0;
        for(size_t i=_startPos; i<=(_toSearch.size()-(_toFind.size()-charsMatched)); i++) {
            if(_toFind.at(charsMatched)==_toSearch.at(i)) {
                charsMatched++;
                if(charsMatched==_toFind.size()) {
                    return i-(charsMatched-1);
                } 
            }
            else {
                charsMatched = 0;
            }
        }

        return std::string::npos;
    }

    template<typename checkType>
    inline int findVectorIndex(std::vector<checkType> vec, checkType toFind) {
        typename std::vector<checkType>::iterator idx = find(vec.begin(), vec.end(), toFind);
        if(idx!=vec.end()) return idx-vec.begin();
        else return -1;
    }

    /**
     * @brief search and find index in std::string vector to given std::string to find
     * @param vec `std::std::vector<std::string>` vector search through
     * @param toFind `std::string` var to find in vector
     * @return index in vector, or `-1` if not found
    */
    inline int findVectorIndex(std::vector<std::string> vec, std::string toFind) {
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
    inline std::vector<int> findVectorIndex(std::vector<std::vector<std::string>> vecvec, std::string toFind, bool printChecks=false) {
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
    inline std::vector<int> findVectorIndex(std::vector<std::vector<std::string>> vecvec, std::vector<std::string> toFind, bool printChecks=false) {
        std::vector<int> indices(2, -1);
        for(std::string subFind: toFind) {
            indices = findVectorIndex(vecvec, subFind, printChecks);
            if(indices[0] != -1) return indices;
        }
        return indices;
    }

    /**
     * @brief Search and find the vector index position of a certain value
     * 
     * @tparam T -data type of elements to look through
     * @param vec vector to search through
     * @param toFind value to find in the vector
     * @return int index of where on `vec` the given `toFind` value exists.
     * @note if the value is not found in the vector then the function will return -1
     */
    template<class T> inline int searchVec(std::vector<T> vec, T toFind) {
        typename std::vector<T>::iterator idx = find(vec.begin(), vec.end(), toFind);
        if(idx!=vec.end()) return idx-vec.begin();
        else return -1;
        // int idx = -1;
        // for(size_t i=0; i<vec.size(); i++) {
        // 	if(vec.at(i)==toFind) {
        //     	idx=i;
        //         break;
        //     }
        // }
        // return idx;
    }

    /**
     * @brief Match vectors for whether they contain the same elements
     * 
     * @tparam typeVar type of the elements in the vectors
     * @param vec0 first vector
     * @param vec1 second vector
     * @param orderImportant whether the order element values have to match
     * @return true if the vectors matched
     * @return false if the vectors didn't match
     */
    template<typename typeVar>
    inline bool match_vectors(std::vector<typeVar> vec0, std::vector<typeVar> vec1, bool orderImportant=false) {
        if(vec0.size()!=vec1.size()) throw std::invalid_argument("the vector's aren't the same size.");

        std::vector<size_t> avoidIdx;
        for(size_t i=0; i<vec0.size(); i++) {
            for(size_t ii=0; ii<vec1.size(); ii++) {
                if(vec0[i]==vec1[ii] && findVectorIndex<size_t>(avoidIdx, ii)!=ii) {
                    avoidIdx.push_back(ii);
                    continue;
                }
                else if(orderImportant) return false;
            }
        }
        if(avoidIdx.size()!=vec0.size()) return false;

        return true;
    }

    /**
     * @brief Find a std::vector inside another std::vector
     * 
     * @tparam typeVar type of the elements in the vectors
     * @param toFind the vector to find
     * @param toSearch the vector to find in / search through.
     * @param orderImportant whether the order of element values have to match the one's found-
     * @return bool for whether all the elements of `toFind` was found in `toSearch` according to previous arguments
     */
    template<typename typeVar>
    inline bool find_vector(std::vector<typeVar> toFind, std::vector<typeVar> toSearch, bool orderImportant=false) {
        if(toFind.size() > toSearch.size()) throw std::invalid_argument("the toFind vector is bigger than toSearch.");


        if(orderImportant) {
            int foundIdx = -1;
            for(size_t i=0; i<=toSearch.size()-toFind.size(); i++) {
                std::vector<typeVar> subVec;
                for(size_t ii=i; ii<toFind.size()+i; ii++) subVec.push_back(toSearch[ii]);

                if(match_vectors<typeVar>(toFind, subVec, orderImportant)) {
                    foundIdx = static_cast<int>(i);
                    break;
                }
            }
            return (foundIdx!=-1? true : false);
        }
        else {
            size_t ignBits = 0; // index to toFind elements to ignore cause they've been found.

            for(size_t i=0; i<toSearch.size(); i++) {
                
                for(size_t ii=0; ii<toFind.size(); ii++) {
                    if(!((ignBits & (1<<ii))>0) && toSearch[i]==toFind[ii]) {
                        /// ii not found in ignBits byte string and it's a found element from toFind
                        
                        ignBits += 1<<ii;
                        break;
                    }
                }
            }
            // std::cout << std::bitset<5>(ignBits) << " ";
            return (ignBits==pow(2, toFind.size())-1? true : false);
        }
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
    inline float findVal(float arrToCheck[], int arrLen, int mode=0) {
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
            if(mode==0 || mode==2)      if(arrToCheck[i]>val) { val=arrToCheck[i]; index=i; }
            else if(mode==1 || mode==3) if(arrToCheck[i]<val) { val=arrToCheck[i]; index=i; }
        }
        if(mode==0 || mode==1) return val;
        else if(mode==2 || mode==3) return index;
        else return -1;
    }

    /**
     * @brief find desired value from vector
     * 
     * @param toCheck container of values to find the desired value of
     * @param toFind what to find:
     * - `0` - biggest value
     * - `1` - smallest value
     * - `2` - index of biggest value
     * - `3` - index of smallest value
     * @return float of the desired value
     */
    inline float findVal(std::vector<float> toCheck, int toFind) {
        int index = 0;
        float val = toCheck[0];
        for(int i=0; i<toCheck.size(); i++) {
            if(toFind==0 || toFind==2) {     if(toCheck[i]>val) { val=toCheck[i]; index=i; } }
            else if(toFind==1 || toFind==3){ if(toCheck[i]<val) { val=toCheck[i]; index=i; } }
        }
        if(toFind==0 || toFind==1) return val;
        else if(toFind==2 || toFind==3) return index;
        else return -1;
    }


    /**
     * @brief Get the value of a desired value
     * 
     * @tparam varType value type
     * @param toCheck the container to check
     * @param toFind what to find.
     * ```
     * - `0` - biggest value
     * - `1` - smallest value
     * ```
     * @return The desired element
     */
    template<typename varType>
    inline varType findVal(std::vector<varType> toCheck, int toFind) {
        int index = 0;
        varType val = toCheck[0];
        for(int i=0; i<toCheck.size(); i++) {
            if(toFind==0 || toFind==2) {    if(toCheck[i]>val) { val=toCheck[i]; index=i; } }
            else if(toFind==1 || toFind==3){if(toCheck[i]<val) { val=toCheck[i]; index=i; } }
        }
        if(toFind==0 || toFind==1) return val;
        else if(toFind==2 || toFind==3) return index;
        else return -1;
    }

    /**
     * @brief Get the index of a desired value
     * 
     * @tparam varType value type
     * @param toCheck the container to check
     * @param toFind what to find.
     * ```
     * - `0` - biggest value
     * - `1` - smallest value
     * ```
     * @return size_t of the desired element index
     */
    template<typename varType>
    inline size_t findIdx(std::vector<varType> toCheck, int toFind) {
        size_t index = 0;
        for(size_t i=1; i<toCheck.size(); i++) {
            switch (toFind) {
            case 0: //max
                if(toCheck[i]>toCheck[index]) {index = i;}
                break;
            case 1: //min
                if(toCheck[i]<toCheck[index]) {index = i;}
                break;
            default:
                throw std::invalid_argument("findIdx: invalid `toFind` argument.");
                break;
            }
        }
        return index;
    }

    template<typename varType>
    inline size_t findIdx(std::initializer_list<varType> toCheck, int toFind) {
        return findIdx<varType>(toCheck, toFind);
    }

    inline std::vector<size_t> _tempRef;
    /**
     * @brief Get the index to desired value from given vector of `std::string`'s
     * 
     * @param _vec `std::vector<std::string>` of the strings to search through.
     * @param _toFind `0`-index to biggest; `1`-index to smallest
     * @return size_t index to desired value/element.
     */
    inline size_t getVal_findString(
        std::vector<std::string> _vec,
        int _toFind
    ) {
        if(_vec.size()==0) throw std::invalid_argument("getVal_findString: std::vector size can't be 0.");
        if(_toFind!=0 && _toFind!=1) throw std::invalid_argument("getVal_findString: invalid code for _toFind.");
        static std::vector<size_t> _sizes;
        _sizes = std::vector<size_t>(_vec.size());
        for(size_t i=0; i<_vec.size(); i++) {
            _sizes[i] = _vec.at(i).size();
        }
        _tempRef = _sizes;
        return findIdx(_sizes, _toFind);
    }

    /**
     * @brief 
     * 
     * @tparam idx_varType type of the variable that's searched for
     * @tparam idx_type type of the return value, i.e. the value that is given by indexing `index_varType`
     * @param toCheck `std::vector<idx_varType>` container that is to be searched, i.e. the hay stack
     * @param toFind what to find:
     * - `0` - biggest value
     * - `1` - smallest value
     * - `2` - index of biggest value
     * - `3` - index of smallest value
     * @param idx index of the data type to search: `toCheck[element][idx]`
     * @return idx_varType of the returned type
     */
    template<typename idx_varType, typename idx_type>
    inline idx_type idx_findVal(
        std::vector<idx_varType> toCheck,
        int toFind,
        size_t idx
    ) {
        int index = 0;
        idx_type val = toCheck[0][idx];
        for(int i=0; i<toCheck.size(); i++) {
            if(toFind==0 || toFind==2)      { if(toCheck[i][idx]>val) { val=toCheck[i][idx]; index=i; } }
            else if(toFind==1 || toFind==3) { if(toCheck[i][idx]<val) { val=toCheck[i][idx]; index=i; } }
        }
        if(toFind==0 || toFind==1)      return val;
        else if(toFind==2 || toFind==3) return index;
        else return -1;
    }


    static inline std::string dummyRef_stringOfVector_resultStrRef;
    /**
     * @brief find specific std::string from `std::std::vector<std::string>` parameter
     * @param stringVec `std::std::vector<std::string>` find find specific std::string from
     * @param id id for what to look for:
     * `0` - find shortest string;
     * `1` - find longest string;
     * @return index of the std::string in the vector
    */
    inline int stringOfVector(
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

    inline void splitString(std::string line, std::string delimiter, float returnArr[], int numVar, bool printVar);

    /**
     * @brief Split and return `line` std::string by each `delimiter`
     * @param line std::string to split.
     * @param delimiter std::string for where to split the std::string `line` by.
     * @param printVar whether to print each delimiter and substring
     * @param causeError whether to end program and throw an error if `line` doesn't contain `delimiter`
    */
    inline std::vector<std::string> splitString(std::string line, std::string delimiter, bool printVar=false, bool causeError=false);
    /**
     * @brief Split `line` std::string by each `delimiter` and pass the result to reference parameter `returnVec`
     * @param line std::string to split.
     * @param delimiter std::string for where to split the std::string `line` by.
     * @param returnVec reference to a `std::vector<std::string>` for which to pass result to
     * @param printVar whether to print each delimiter and substring
    */
    inline void splitString(std::string line, std::string delimiter, std::vector<std::string> &returnVec, bool printVar);
    /**
     * @brief Split `line` std::string by each `delimiter` and pass the result to a pointer parameter `returnPtr`
     * @param line std::string to split.
     * @param delimiter std::string for where to split the std::string `line` by.
     * @param returnPtr pointer to a `std::vector<std::string>` for which to pass result to
     * @param printVar whether to print each delimiter and substring
    */
    inline void splitString(std::string line, std::string delimiter, std::vector<std::string> *returnPtr, bool printVar);

    /***
     * @brief Get size of terminal window
     * @param width reference to integer variable for width
     * @param height reference to integer variable for height
     * @return `0`-successful; `-1`-error
    */
    inline int getTermSize(int &width, int &height) {
#if _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int columns, rows;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        width   = csbi.srWindow.Right   - csbi.srWindow.Left    + 1;
        height  = csbi.srWindow.Bottom  - csbi.srWindow.Top     + 1;
#else
        struct winsize winDim;
        if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&winDim)==-1) {
            return -1;
        }
        width   = winDim.ws_col;
        height  = winDim.ws_row;
#endif
        return 0;
    }

    inline std::string getDate(bool addNewline = true) {
        time_t currDate = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string _out = ctime(&currDate);
        if(addNewline) return _out;
        else {
            return _out.substr(0, _out.length()-1);
        }
    }

    inline std::string dateToStr(
        tm _date
    ) {
        return (
            formatNumber(_date.tm_mday, 2, 0, "right", true) + "-" +
            formatNumber(_date.tm_mon , 2, 0, "right", true) + "-" +
            formatNumber(_date.tm_year, 4, 0, "right", true)
        );
    } 

    /**
     * @brief method to allow ANSI printing string with newlines
     * @param toPrint string with newlines to print (doesn't need to have newlines)
     * @param xPos terminal x position to print on
     * @param yPos terminal y position to print on
     * @param flushEnd whether to flush `std::cout`
     * @param x_method the method to set the new x coordiante by: `"abs"`-give absolute coordinates, `"rel"`-relative to previously given coordinates.
     * The previous line width is stored. If "rel" and `xPos==-1` then it'll print at same x pos as prev func call minux 1.
     * @param y_method the method to set hte new y coordinate by: `"abs"`-give absolute coordinates, `"rel"`-relative to previously given coordinates.
     * The previous line height pos is stored. If "rel" and `yPos==-1` then it'll print at same y pos as prev func call.
     * @note If `y_method=="rel"` and `yPos==0` then it'll print on a new line rfom the previous func call position.
     * @note To continue writing exactly from previous func call, `xPos==0`, `yPos==-1`, `x_method=="rel"`, `y_method=="rel"`
    */
    inline void ANSI_mvprint(
        int posX,
        int posY,
        std::string printText,
        bool flushEnd = true,
        std::string x_method = "abs",
        std::string y_method = "abs",
        bool initClearScr = false
    ) {
        static int prevPos[2] = {0, 0};

        if(x_method=="rel") posX = prevPos[0]+posX;
        if(y_method=="rel") posY = prevPos[1]+posY;

        std::string ansiCode = "\x1B[";

        if(initClearScr) {
            std::cout<<ansiCode<<"2J";
            std::cout.flush();
        }

        size_t tPos=0, ePos=0;
        int rowCount= 0;
        int rowLen  = 0;
        std::string _substring = "";
        while(printText.find('\n', tPos)!=std::string::npos) {
            ePos = printText.find('\n', tPos);
            _substring = printText.substr(tPos, ePos-tPos);
            std::cout<<ansiCode<<posY+rowCount<<";"<<posX<<"H"<<_substring;
            tPos = ePos+1;
            rowCount++;
        }
        _substring = printText.substr(tPos, printText.length());
        std::cout<<ansiCode<<posY+rowCount<<";"<<posX<<"H"<<_substring;
        rowCount++;

        // std::cout<<ansiCode<<posY<<";"<<posX<<"H"<<printText;
        if(flushEnd) std::cout.flush();
        prevPos[0] = posX + _substring.length();
        prevPos[1] = posY + rowCount;
    }

    inline void baseAnsiPrint(
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
    inline void ansiPrint(
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
    inline void ansiPrint(
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
    inline void ansiPrint(
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
    inline float toDegrees(float radians) { return (radians*180)/M_PI; }
    /// @brief Convert degrees to radians
    /// @param degrees degrees to convert. type: float()
    /// @return return radians. type: float()
    inline float toRadians(float degrees) { return float(degrees*M_PI)/180; }

    /**
     * @brief Get the absolute path of the current directory
     * @param inclEndSlash whether to include `/` at the end of return path string
     * @return std::string of path:
    */
    inline std::string getFileCWD(bool inclEndSlash=true) {
#if _WIN32
        char cwd[MAX_PATH];
        if(_getcwd(cwd, sizeof(cwd)) != NULL)
#else
        char cwd[PATH_MAX];
        if(getcwd(cwd, sizeof(cwd)) != NULL)
#endif
        {
            // std::cout<<cwd<<std::endl;
            std::string returStr = cwd;
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
    inline bool isFile(std::string filename) {
#if _WIN32
        std::wstring wstr(filename.begin(), filename.end());
        DWORD dwAttrib = GetFileAttributes(filename.c_str());
        return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
        struct stat sb;
        if(stat(filename.c_str(), &sb)==0 && !(sb.st_mode & S_IFDIR)) {
            return true;
        }
        else {
            return false;
        }
#endif
    }

    /**
     * @brief check if `dirname` directive exists
     * @param dirname name of directive (with or without path) to check
     * @return boolean for if it exists
    */
    inline bool isDir(std::string dirname) {
#if _WIN32
        std::wstring wstr(dirname.begin(), dirname.end());
        DWORD dwAttrib = GetFileAttributes(dirname.c_str());
        return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
        struct stat sb;
        if(stat(dirname.c_str(), &sb)==0) {
            return true;
        }
        else {
            return false;
        }
#endif  
    }

    /// @brief Solve distance value between two coordinates in a 3D spacec
    /// @param p1 float()[3]: point 1 coordinate {x, y, z}
    /// @param p2 float()[3]: point 2 coordinate {x, y, z}
    /// @return float() type of absolute straight distance
    inline float get3dDistance(
        float p1[3],
        float p2[3]
    ) {
        return sqrt(pow(p2[0]-p1[0],2) + pow(p2[1]-p1[1],2) + pow(p2[2]-p1[2],2));
    }


    /**
     * @brief Get the square root of the sum of the deltas between each axis value of both points
     * 
     * @tparam _pos_container container that can be indexed that stores the points coordinates for `numDim` number of "dimensions"
     * @param numDim number of dimensions to solve for
     * @param point_1 
     * @param point_2 
     * @return float of the distance between the two points
     */
    template<typename _pos_container>
    inline float getNDimDistance(
        int numDim,
        _pos_container point_1,
        _pos_container point_2
    ) {
        float sumDelta=0;
        for(int dim=0; dim<numDim; dim++) {
            sumDelta += pow(point_2[dim]-point_1[dim], 2);
        }
        return sqrt(sumDelta);

    }




    /// @brief Solve whether a number is positive or negative (same as int(var/abs(var)))
    /// @param var variable to check
    /// @return 1 if var>0; -1 if var<0; 0 if var==0
    inline int PoN(float var) {
        if(var>0) return 1;
        else if(var<0) return -1;
        else {
            return 1;
            std::cout << "PoN(): \"" << var << "\" is not a number\n";
            return 0;
        }
    }


    /// @brief Find index to closest value in arr
    /// @param arr std::vector<float>: the hay in which to find the needle in
    /// @param needle value to find the closest to in the array
    /// @param printVar whether to print results
    /// @return index of result (closest value)
    inline int getClosestValIdx(std::vector<float> arr, float needle, bool printVar=false) {
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
    inline int getClosestValIdx(float arr[], int arrLen, float needle, bool printVar=false) {
        std::vector<float> diffVec;
        std::vector<int> foundIdx;
        float minVar;

        for(int i=0; i<arrLen; i++) { diffVec.push_back(abs(arr[i]-needle)); }
        minVar = *min_element(diffVec.begin(), diffVec.end());
        for(int i=0; i<diffVec.size(); i++) { if(diffVec.at(i)==minVar) foundIdx.push_back(i); }
        if(foundIdx.size()==0) return -1;
        return foundIdx.at(0);
    }


    inline void splitString(
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

    inline std::vector<std::string> splitString(
        std::string line,
        std::string delimiter,
        bool printVar,
        bool causeError
    ) {
        if(printVar) std::cout << "--- \"" << line << "\"\n";
        /// `std::vector<std::string>` of the strings containing the results
        std::vector<std::string> resultStrings;
        size_t idx0 = 0;
        size_t idx1 = line.find(delimiter, idx0);

        if(idx1==std::string::npos) {
            resultStrings.push_back(line);
            if(causeError) throw std::runtime_error("std::vector<std::string> splitString(std::string, std::string, bool, bool) delimiter doesn't exist in `line`");
            if(printVar) std::cout << "error: splitString: no delimiter \"" << delimiter << "\" found. Returning empty vector\n";
            return resultStrings;
        }

        while(true) {
            resultStrings.push_back(line.substr(idx0, idx1-idx0));
            if(idx1==std::string::npos) break;
            for(;line.substr(idx1+delimiter.length(), delimiter.length())==delimiter; idx1+=delimiter.length());
            idx0 = idx1+delimiter.length();
            if(idx0==line.length()) break;
            idx1 = line.find(delimiter, idx0);
        }

        if(printVar) std::cout << "---";
        return resultStrings;
    }
    inline void splitString(
        std::string line,
        std::string delimiter,
        std::vector<std::string> &returnVec,
        bool printVar=false
    ) {
        returnVec.clear();
        returnVec = splitString(line,delimiter,printVar);
    }
    inline void splitString(
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
    inline std::string replaceSubstr(std::string text, std::string toReplace, std::string replaceTo) {
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
    inline void replaceSubstr(std::string* text, std::string toReplace, std::string replaceTo) {
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
    inline void fillCharArray(
        float inpVal,
        int idx_start,
        char *toSend_arr,
        int width=0,
        int precision=0,
        bool leftAlign=false
    ) {
        // char fullTemp[width+1];
        std::vector<char> fullTemp(width+1, 0);
        
        if(leftAlign) snprintf(fullTemp.data(), width+1, "%-*.*f", width, precision, inpVal);
        else snprintf(fullTemp.data(), width+1, "%*.*f", width, precision, inpVal);
        
        for(int i=0; i<fullTemp.size(); i++) {
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
    inline std::string formattedFloat_c(
        float inpVal,
        int width=0,
        int precision=0,
        bool leftAlign=false
    ) {
        std::vector<char> temp(width+1, 0);
        if(leftAlign) sprintf(temp.data(), "%-*.*f", width, precision, inpVal);
        else sprintf(temp.data(), "%*.*f", width, precision, inpVal);

        std::string tempStr(temp.data());
        return tempStr;
    }
    inline void formattedFloat_c(
        float inpVal,
        std::string *strVarPtr,
        int width=0,
        int precision=0,
        bool leftAlign=false
    ) {
        *strVarPtr = formattedFloat_c(inpVal, width, precision, leftAlign);
    }

    template<class T>
    inline std::string FormatWithSymbol(T value, std::string formatSymbol=" ") {
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
    inline std::string formatNumber(T value, int varPrecision=2) {
        std::stringstream tempStream;
        tempStream << std::fixed << std::setprecision(varPrecision) << value;
        return tempStream.str();
    }
    template<class T>
    inline std::string formatNumber(
        T value,
        int strWidth,
        int varPrecision,
        std::string align,
        bool numberFill
    ) {
        std::stringstream outStream, _temp;
        int fillZeros = 0;
        if(numberFill && align=="right") {
            _temp << std::fixed;
            _temp << std::setprecision(varPrecision) << value;
            if(static_cast<int>(_temp.str().length()) < strWidth) fillZeros = strWidth - static_cast<int>(_temp.str().length());
        }
        outStream << std::fixed;
        outStream << std::boolalpha;
        if(align=="left") outStream<<std::left;
        else if(align=="right") outStream<<std::right;
        outStream << std::setw(strWidth - fillZeros);
        if(numberFill && align=="right") outStream << std::string(fillZeros, '0');
        // outStream << (align=="left"? std::left : std::right);
        outStream << std::setprecision(varPrecision) << value;

        return outStream.str();
    }

    template<class T>
    inline std::string formatContainer(
        T _container,
        int strWidth,
        int varPrecision,
        std::string align = "right",
        bool numberFill = false,
        char openSymb   = '{',
        char closeSymb  = '}',
        char _sepSymb   = ','
    ) {
        std::string _out(1, openSymb);
        for(auto itr=_container.begin(); itr!=_container.end(); ++itr) {
            _out += formatNumber(*itr, strWidth, varPrecision, align, numberFill);
            if(itr!=--_container.end()) openSymb += _sepSymb;
        }
        return _out + closeSymb;
    }

    template<class T>
    inline std::string formatVector(
        std::vector<T>  _container,
        int             _strWidth   = 0,
        int             _precision  = 1,
        std::string     _align      = "right",
        bool            _numberFill = false,
        char            _openSymb   = '{',
        char            _closeSymb  = '}',
        char            _sepSymb    = ','
    ) {
        std::string _out(1, _openSymb);
        for(size_t i=0; i<_container.size(); i++) {
            _out += formatNumber(_container[i], _strWidth, _precision, _align, _numberFill);
            if(i<_container.size()-1) _out += _sepSymb;
        }
        return _out + _closeSymb;
    }

    template<class T>
    inline std::string formatContainer1(
        T _container,
        size_t contSize,
        int strWidth,
        int varPrecision,
        std::string align = "right",
        bool numberFill = false,
        char openSymb   = '{',
        char closeSymb  = '}',
        char _sepSymb   = ','
    ) {
        std::string _out(1, openSymb);
        for(size_t i=0; i<contSize; i++) {
            _out += formatNumber(_container[i], strWidth, varPrecision, align, numberFill);
            if(i<contSize-1) _out += _sepSymb;
        }
        return _out + closeSymb;
    }


    /// @brief create a progress bar on terminal
    /// @param progress current progress made
    /// @param total_val total progress which reesembles 100&
    /// @param printBar whether to print the progress bar
    /// @param progFin whether the progress is finished (used to get total duration)
    /// @param interval the time interval between each progressBar update
    /// @param symbolIndex what symbol to use {"■", "⬛", "▉", "▉", "█"}
    /// @return percent of the progress made
    inline float progressBar(
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


    inline std::vector<int> convert_RGB_HSV(
        std::vector<int> _RGB
    ) {
        std::vector<float> RGB_p{
            static_cast<float>(_RGB[0])/255,
            static_cast<float>(_RGB[1])/255,
            static_cast<float>(_RGB[2])/255
        };
        std::vector<int> HSV(3, 0);
        size_t maxIdx = findIdx<float>(RGB_p, 0);
        size_t minIdx = findIdx<float>(RGB_p, 1);

        int delta = _RGB[maxIdx]-_RGB[minIdx];

        HSV[2] = static_cast<int>(100*RGB_p[maxIdx]);
        HSV[1] = static_cast<int>(100*(HSV[2]==0? 0 : delta/RGB_p[maxIdx]));
        switch (maxIdx) {
            case 0:
                HSV[0] = static_cast<int>(60*(delta==0? 0 : ((RGB_p[1]-RGB_p[2])/(delta)+0)));
                break;
            case 1:
                HSV[0] = static_cast<int>(60*(delta==0? 0 : ((RGB_p[2]-RGB_p[0])/(delta)+2)));
                break;
            case 2:
                HSV[0] = static_cast<int>(60*(delta==0? 0 : ((RGB_p[0]-RGB_p[1])/(delta)+4)));
                break;
        }
        if(HSV[0]<0) HSV[0]+=360;

        return HSV;
    }

    inline std::vector<int> convert_HSV_RGB(
        std::vector<int> HSV
    ) {
        std::vector<int> _RGB(3, 0);
        std::vector<float> RGB_p(3, 0);
        std::vector<float> HSV_p{
            static_cast<float>(HSV[0]),
            static_cast<float>(HSV[1])/100,
            static_cast<float>(HSV[2])/100
        };

        float C = HSV_p[2] * HSV_p[1];
        float X = C * (1 - abs((static_cast<int>(HSV_p[0])/60)%2 -1));
        float m = HSV_p[2] - C;

        if(HSV_p[0] < 60) {
            RGB_p[0] = C;
            RGB_p[1] = X;
            RGB_p[2] = 0;
        }
        else if(HSV_p[0] < 120) {
            RGB_p[0] = X;
            RGB_p[1] = C;
            RGB_p[2] = 0;
        }
        else if(HSV_p[0] < 180) {
            RGB_p[0] = 0;
            RGB_p[1] = C;
            RGB_p[2] = X;
        }
        else if(HSV_p[0] < 240) {
            RGB_p[0] = 0;
            RGB_p[1] = X;
            RGB_p[2] = C;
        }
        else if(HSV_p[0] < 300) {
            RGB_p[0] = X;
            RGB_p[1] = 0;
            RGB_p[2] = C;
        }
        else {
            RGB_p[0] = C;
            RGB_p[1] = 0;
            RGB_p[2] = X;
        }

        _RGB[0] = (RGB_p[0]+m)*static_cast<float>(255);
        _RGB[1] = (RGB_p[1]+m)*static_cast<float>(255);
        _RGB[2] = (RGB_p[2]+m)*static_cast<float>(255);

        return _RGB;
    }


// }

#endif
