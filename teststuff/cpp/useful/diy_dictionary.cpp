
#include "diy_dictionary.hpp"
#include <HC_useful/useful.hpp>

/**
 * @brief common function to check if a `key` exists in `diy_dict::keys` vector/container
 * 
 * @param key the `std::string` label/key to check if it exists in the vecot
 * @param verbose whether to print the output:
 * `0` = false; `1` = true; `-1` = {use default}/`diy_dict::arg_searchVec_verbose`
 * @return `int` of idx for where that key exists in `diy_dict::keys` vector; returns `-1` if key doesn't exist.
 */
int diy_dict::check_existence(std::string key, int verbose=-1) {
    if(verbose==-1) verbose = verbose = arg_searchVec_verbose;
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(
        keys, key, arg_searchVec_numThreads, arg_searchVec_threadLen, false, arg_searchVec_checkSpacing, verbose);
    return pos[0];
}
/**
 * @brief check if `key` exist and if not then extend `keys` and `datatype` vectors
 * 
 * @param key string to check if exists already and if not add this to common vector `keys`
 * @param varType integer to type of value/ptr to add according to `types` definition
 * @return int value for success or not: `0`-successfully added `key` and `datatype` to vectors; `1`-unsuccessful. key already exists
 */
int diy_dict::extend_reg(std::string key, int varType) {
    if(check_existence(key) != -1) return 1;
    keys.push_back(key);
    datatype.push_back(varType);
    return 0;
}


template<typename T> std::string diy_dict::str_export_v1_subFunc(std::vector<T> vec1_inp) {

}
template<typename T> std::string diy_dict::str_export_v2_subFunc(std::vector<std::vector<T>> vec2_inp) {

}


/**
 * @brief Export the value of the stored value or pointer as a string
 * 
 * @param key `std::string` identifier to stored value/pointer
 * @param codedInsert string to insert at `i` index, isolated by `$` symbol after: format `"i$text"`. ex:"5$test" will insert string `text` at pos `5` after export string creation.(1)
 * @param align where to align numbers or text relative to "empty" space char's created by format parameters `width`, `padding`, `prettyPrint`.
 * {`"left"`, `"center"`, "`right`""}. (2)
 * @param decimals decimal precision: if `key` result contains number with decimals, then `decimals` number of decimals will be rounded to.
 * @param width minimum number of characters to be filled in the exported string.
 * If `key` value converted to string takes up less number of char's than `width`, then the remaining space
 * will be filled with char given in `emptySpace` argument in accordance with `align`, `padding`, `padding` and `prettyPrint`(3).
 * If `width`<=`-1` then width not set.
 * @param padding number of `emptySpace` characters to add on both start and end of `width` defined string (4).
 * @param prettyPrint modes to methods for creating a string of non-simple data types ex. `std::vector`:
 *  `0`-in-line:    every element in a container comes after eachother. 
 *  `1`-new-line:   newline every element.
 *  `2`-new-line2:  newline every element except the lowest "level" of container which will be "in-line".
 * @param emptySpace character to fill the "empty" space in accordance with string format parameter arguments
 * @param left_indent if formatted string of `std::vector` takes up multiple lines('\n') then a left indent / tab will be added with `left_indent` number of `emptySpace` chars
 * @return `std::string` of the exported string
 * 
 * @note (1) if created string take up multiple lines or `left_indent` then the inserted index `i` will ignore `left_indent`/`\n` characters but it will include `padding` and `width`.
 * @note (2) if value is a `std::vector` then only the elements will be aligned and not the curly braces.
 * @note (3) if value is a `std::vector` then `width` will be applied to the elements and not the curly braces
 * @note (4) if value is a `std::vector` then `padding` will be outside the curly braces and not the elements, same goes for if `prettyPrint` is set to include new-lines, 
 * just now it'll apply to each `container element` if container is multidimensional
 */
std::string diy_dict::str_export(
    std::string key,
    std::string codedInsert = "",
    std::string align = "right",
    int decimals = 2,
    int width = -1,
    int padding = 0,
    int prettyPrint = 0,
    char emptySpace = ' ',
    int left_indent = 0
) {
    int pos = check_existence(key);
    if(pos==-1) return "";
    std::string finalStr = "";
    std::string tempStr = "";

    int type = datatype[pos];
    int vecType = 0; //level of vec
    bool isPtr = type % 10;
    float type_deriv2 = 0;
    
    static auto boolLambda = [](bool boolVar) {
        if(boolVar) return std::string("true");
        return std::string("true");
    };

    if(type<100) {}
    else if(type<200) { type_deriv2 = static_cast<float>(type-100); vecType = 1; }
    else if(type<300) { type_deriv2 = static_cast<float>(type-200); vecType = 2; }
    

    switch (static_cast<int>(floor(type_deriv2/10))) {
    case 0: //bool
        if(vecType==0 && isPtr) tempStr = std::string(padding,emptySpace) + boolLambda(*values_0_bool_p[pos]) + std::string(padding,emptySpace);
        if(vecType==0 && !isPtr) tempStr = std::string(padding,emptySpace) + boolLambda(values_0_bool[pos]) + std::string(padding,emptySpace);
        
        break;
    case 1: //int

    case 2: //float
    case 3: //double
    case 4: //char
    case 5: //std::string
    default:
        break;
    }
    

    return finalStr;
}


int diy_dict::get_type(std::string key) {
    int pos = check_existence(key);
    return datatype[pos];
}


int diy_dict::add(std::string key, bool value) {
    if(extend_reg(key, 0)==1) return 1;
    values_0_bool.push_back(value);
    idx.push_back(values_0_bool.size());
    return 0;
}
int diy_dict::add(std::string key, int value) {
    if(extend_reg(key, 10)==1) return 1;
    values_0_int.push_back(value);
    idx.push_back(values_0_int.size());
    return 0;
}
int diy_dict::add(std::string key, float value) {
    if(extend_reg(key, 20)==1) return 1;
    values_0_float.push_back(value);
    idx.push_back(values_0_float.size());
    return 0;
}
int diy_dict::add(std::string key, double value) {
    if(extend_reg(key, 30)==1) return 1;
    values_0_double.push_back(value);
    idx.push_back(values_0_double.size());
    return 0;
}
int diy_dict::add(std::string key, char value) {
    if(extend_reg(key, 40)==1) return 1;
    values_0_char.push_back(value);
    idx.push_back(values_0_char.size());
    return 0;
}
int diy_dict::add(std::string key, std::string value) {
    if(extend_reg(key, 50)==1) return 1;
    values_0_string.push_back(value);
    idx.push_back(values_0_string.size());
    return 0;
}

int diy_dict::add(std::string key, bool* ptr) {
    if(extend_reg(key, 1)==1) return 1;
    values_0_bool_p.push_back(ptr);
    idx.push_back(values_0_bool_p.size());
    return 0;
}
int diy_dict::add(std::string key, int* ptr) {
    if(extend_reg(key, 11)==1) return 1;
    values_0_int_p.push_back(ptr);
    idx.push_back(values_0_int_p.size());
    return 0;
}
int diy_dict::add(std::string key, float* ptr) {
    if(extend_reg(key, 21)==1) return 1;
    values_0_float_p.push_back(ptr);
    idx.push_back(values_0_float_p.size());
    return 0;
}
int diy_dict::add(std::string key, double* ptr) {
    if(extend_reg(key, 31)==1) return 1;
    values_0_double_p.push_back(ptr);
    idx.push_back(values_0_double_p.size());
    return 0;
}
int diy_dict::add(std::string key, char* ptr) {
    if(extend_reg(key, 41)==1) return 1;
    values_0_char_p.push_back(ptr);
    idx.push_back(values_0_char_p.size());
    return 0;
}
int diy_dict::add(std::string key, std::string* ptr) {
    if(extend_reg(key, 51)==1) return 1;
    values_0_string_p.push_back(ptr);
    idx.push_back(values_0_string_p.size());
    return 0;
}


int diy_dict::add(std::string key, std::vector<bool> value) {
    if(extend_reg(key, 100)==1) return 1;
    values_1_bool.push_back(value);
    idx.push_back(values_1_bool.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<int> value) {
    if(extend_reg(key, 110)==1) return 1;
    values_1_int.push_back(value);
    idx.push_back(values_1_int.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<float> value) {
    if(extend_reg(key, 120)==1) return 1;
    values_1_float.push_back(value);
    idx.push_back(values_1_float.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<double> value) {
    if(extend_reg(key, 130)==1) return 1;
    values_1_double.push_back(value);
    idx.push_back(values_1_double.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<char> value) {
    if(extend_reg(key, 140)==1) return 1;
    values_1_char.push_back(value);
    idx.push_back(values_1_char.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::string> value) {
    if(extend_reg(key, 150)==1) return 1;
    values_1_string.push_back(value);
    idx.push_back(values_1_string.size());
    return 0;
}

int diy_dict::add(std::string key, std::vector<bool>* ptr) {
    if(extend_reg(key, 101)==1) return 1;
    values_1_bool_p.push_back(ptr);
    idx.push_back(values_1_bool_p.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<int>* ptr) {
    if(extend_reg(key, 111)==1) return 1;
    values_1_int_p.push_back(ptr);
    idx.push_back(values_1_int_p.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<float>* ptr) {
    if(extend_reg(key, 121)==1) return 1;
    values_1_float_p.push_back(ptr);
    idx.push_back(values_1_float_p.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<double>* ptr) {
    if(extend_reg(key, 131)==1) return 1;
    values_1_double_p.push_back(ptr);
    idx.push_back(values_1_double_p.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<char>* ptr) {
    if(extend_reg(key, 141)==1) return 1;
    values_1_char_p.push_back(ptr);
    idx.push_back(values_1_char_p.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::string>* ptr) {
    if(extend_reg(key, 151)==1) return 1;
    values_1_string_p.push_back(ptr);
    idx.push_back(values_1_string_p.size());
    return 0;
}


int diy_dict::add(std::string key, std::vector<std::vector<bool>> value) {
    if(extend_reg(key, 200)==1) return 1;
    values_2_bool.push_back(value);
    idx.push_back(values_2_bool.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<int>> value) {
    if(extend_reg(key, 210)==1) return 1;
    values_2_int.push_back(value);
    idx.push_back(values_2_int.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<float>> value) {
    if(extend_reg(key, 220)==1) return 1;
    values_2_float.push_back(value);
    idx.push_back(values_2_float.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<double>> value) {
    if(extend_reg(key, 230)==1) return 1;
    values_2_double.push_back(value);
    idx.push_back(values_2_double.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<char>> value) {
    if(extend_reg(key, 240)==1) return 1;
    values_2_char.push_back(value);
    idx.push_back(values_2_char.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<std::string>> value) {
    if(extend_reg(key, 250)==1) return 1;
    values_2_string.push_back(value);
    idx.push_back(values_2_string.size());
    return 0;
}

int diy_dict::add(std::string key, std::vector<std::vector<bool>>* ptr) {
    if(extend_reg(key, 201)==1) return 1;
    values_2_bool_p.push_back(ptr);
    idx.push_back(values_2_bool_p.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<int>>* ptr) {
    if(extend_reg(key, 211)==1) return 1;
    values_2_int_p.push_back(ptr);
    idx.push_back(values_2_int_p.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<float>>* ptr) {
    if(extend_reg(key, 221)==1) return 1;
    values_2_float_p.push_back(ptr);
    idx.push_back(values_2_float_p.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<double>>* ptr) {
    if(extend_reg(key, 231)==1) return 1;
    values_2_double_p.push_back(ptr);
    idx.push_back(values_2_double_p.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<char>>* ptr) {
    if(extend_reg(key, 241)==1) return 1;
    values_2_char_p.push_back(ptr);
    idx.push_back(values_2_char_p.size());
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<std::string>>* ptr) {
    if(extend_reg(key, 251)==1) return 1;
    values_2_string_p.push_back(ptr);
    idx.push_back(values_2_string_p.size());
    return 0;
}




bool        diy_dict::get0_bool_  (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==  0) return values_0_bool.at(idx[pos[0]]);
    return NULL;
}
int         diy_dict::get0_int_   (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]== 10) return values_0_int.at(idx[pos[0]]);
    return NULL;
}
float       diy_dict::get0_float_ (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]== 20) return values_0_float.at(idx[pos[0]]);
    return NULL;
}
double      diy_dict::get0_double_(std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]== 30) return values_0_double.at(idx[pos[0]]);
    return NULL;
}
char        diy_dict::get0_char_  (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]== 40) return values_0_char.at(idx[pos[0]]);
    return NULL;
}
std::string diy_dict::get0_string_(std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]== 50) return values_0_string.at(idx[pos[0]]);
    return NULL;
}

bool*        diy_dict::get0_boolP  (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==  1) return values_0_bool_p.at(idx[pos[0]]);
    return NULL;
}
int*         diy_dict::get0_intP   (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]== 11) return values_0_int_p.at(idx[pos[0]]);
    return NULL;
}
float*       diy_dict::get0_floatP (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]== 21) return values_0_float_p.at(idx[pos[0]]);
    return NULL;
}
double*      diy_dict::get0_doubleP(std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]== 31) return values_0_double_p.at(idx[pos[0]]);
    return NULL;
}
char*        diy_dict::get0_charP  (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]== 41) return values_0_char_p.at(idx[pos[0]]);
    return NULL;
}
std::string* diy_dict::get0_stringP(std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]== 51) return values_0_string_p.at(idx[pos[0]]);
    return NULL;
}


std::vector<bool>           diy_dict::get1_bool_  (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==100) return values_1_bool.at(idx[pos[0]]);
    return std::vector<bool>();
}
std::vector<int>            diy_dict::get1_int_   (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==110) return values_1_int.at(idx[pos[0]]);
    return std::vector<int>();
}
std::vector<float>          diy_dict::get1_float_ (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==120) return values_1_float.at(idx[pos[0]]);
    return std::vector<float>();
}
std::vector<double>         diy_dict::get1_double_(std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==130) return values_1_double.at(idx[pos[0]]);
    return std::vector<double>();
}
std::vector<char>           diy_dict::get1_char_  (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==140) return values_1_char.at(idx[pos[0]]);
    return std::vector<char>();
}
std::vector<std::string>    diy_dict::get1_string_(std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==150) return values_1_string.at(idx[pos[0]]);
    return std::vector<std::string>();
}

std::vector<bool>*          diy_dict::get1_boolP  (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==101) return values_1_bool_p.at(idx[pos[0]]);
    return nullptr;
}
std::vector<int>*           diy_dict::get1_intP   (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==111) return values_1_int_p.at(idx[pos[0]]);
    return nullptr;
}
std::vector<float>*         diy_dict::get1_floatP (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==121) return values_1_float_p.at(idx[pos[0]]);
    return nullptr;
}
std::vector<double>*        diy_dict::get1_doubleP(std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==131) return values_1_double_p.at(idx[pos[0]]);
    return nullptr;
}
std::vector<char>*          diy_dict::get1_charP  (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==141) return values_1_char_p.at(idx[pos[0]]);
    return nullptr;
}
std::vector<std::string>*   diy_dict::get1_stringP(std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==151) return values_1_string_p.at(idx[pos[0]]);
    return nullptr;
}


std::vector<std::vector<bool>>        diy_dict::get2_bool_  (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==200) return values_2_bool.at(idx[pos[0]]);
    return std::vector<std::vector<bool>>();
}
std::vector<std::vector<int>>         diy_dict::get2_int_   (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==210) return values_2_int.at(idx[pos[0]]);
    return std::vector<std::vector<int>>();
}
std::vector<std::vector<float>>       diy_dict::get2_float_ (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==220) return values_2_float.at(idx[pos[0]]);
    return std::vector<std::vector<float>>();
}
std::vector<std::vector<double>>      diy_dict::get2_double_(std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==230) return values_2_double.at(idx[pos[0]]);
    return std::vector<std::vector<double>>();
}
std::vector<std::vector<char>>        diy_dict::get2_char_  (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==240) return values_2_char.at(idx[pos[0]]);
    return std::vector<std::vector<char>>();
}
std::vector<std::vector<std::string>> diy_dict::get2_string_(std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==250) return values_2_string.at(idx[pos[0]]);
    return std::vector<std::vector<std::string>>();
}

std::vector<std::vector<bool>>*        diy_dict::get2_boolP  (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==201) return values_2_bool_p.at(idx[pos[0]]);
    return nullptr;
}
std::vector<std::vector<int>>*         diy_dict::get2_intP   (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==211) return values_2_int_p.at(idx[pos[0]]);
    return nullptr;
}
std::vector<std::vector<float>>*       diy_dict::get2_floatP (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==221) return values_2_float_p.at(idx[pos[0]]);
    return nullptr;
}
std::vector<std::vector<double>>*      diy_dict::get2_doubleP(std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==231) return values_2_double_p.at(idx[pos[0]]);
    return nullptr;
}
std::vector<std::vector<char>>*        diy_dict::get2_charP  (std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==241) return values_2_char_p.at(idx[pos[0]]);
    return nullptr;
}
std::vector<std::vector<std::string>>* diy_dict::get2_stringP(std::string key) {
    std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(keys, key, -1, 100, false, 1);
    if(pos[0]!=-1 && datatype[pos[0]]==251) return values_2_string_p.at(idx[pos[0]]);
    return nullptr;
}


/// @brief Convert decimal number to std::string with set decimal numbers and minimum total width
/// @tparam T 
/// @param value decimal number to convert
/// @param strWidth minimum width for the string
/// @param varPrecision decimal precision
/// @param align whether to align value with left or right side: {`"left"`, `"right"`},
/// @return returns formatted string
template<class T>
std::string diy_dict::formatNumber(
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
