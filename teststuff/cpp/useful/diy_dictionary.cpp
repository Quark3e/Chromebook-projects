
#include "diy_dictionary.hpp"


diy_dict::diy_dict(/* args */) {

}

diy_dict::~diy_dict() {}


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


std::string diy_dict::bool_string(bool boolVar) {
    if(boolVar) return "true";
    return "false";
}


/**
 * @brief Export the value of the stored value or pointer as a string
 * 
 * @param key `std::string` identifier to stored value/pointer
 * @param codedInsert string to insert at `i` index, isolated by `$` symbol after: format `"i$text"`. ex:"5$test" will insert string `text` at pos `5` after export string creation.(1)
 * @param align where to align numbers or text relative to "empty" space char's created by format parameters `width`, `padding`, `prettyPrint`.
 * {`"left"`, "`right`""}. (2)
 * @param decimals decimal precision: if `key` result contains number with decimals, then `decimals` number of decimals will be rounded to.
 * @param width minimum number of characters to be filled in the exported string.
 * If `key` value converted to string takes up less number of char's than `width`, then the remaining space
 * will be filled with char given in `emptySpace` argument in accordance with `align`, `padding`, `padding` and `prettyPrint`(3).
 * If `width`<=`-1` then width not set.
 * @param padding number of `emptySpace` characters to add on both start and end of `width` defined string (4).
 * @param prettyPrint modes to methods for creating a string of non-simple data types ex. `std::vector`:
 *  `0`-in-line:    every element in a container comes after eachother. 
 *  `1`-new-line:   newline every element.
 *  `2`-new-line2:  newline every element except the lowest "level" of container which will be "in-line" unless vector is only 1 dimensional (ex. `std::vector<T>` compared to `std::vector<std::vector<T>>`)
 * @param emptySpace character to fill the "empty" space in accordance with string format parameter arguments
 * @param left_indent if formatted string of `std::vector` takes up multiple lines('\n') then a left indent / tab will be added with `left_indent` number of `emptySpace` chars for each element
 * @return `std::string` of the exported string
 * @note - (1) if created string take up multiple lines or `left_indent` then the inserted index `i` will include `left_indent`/`\n` characters along with `padding` and `width`.
 * @note - (2) if value is a `std::vector` then only the elements will be aligned and not the curly braces.
 * @note - (3) if value is a `std::vector` then `width` will be applied to the elements and not the curly braces
 * @note - (4) if value is a `std::vector` then `padding` will be outside the curly braces and not the elements, same goes for if `prettyPrint` is set to include new-lines, 
 * just now it'll apply to each `container element` if container is multidimensional
 */
std::string diy_dict::str_export(
    std::string key,
    std::string codedInsert,
    std::string align,
    int decimals,
    int width,
    int padding,
    int prettyPrint,
    char emptySpace,
    int left_indent
) {
    int pos = check_existence(key);
    if(pos==-1) return "";
    std::string tempStr = "";

    int type = datatype[pos];
    int vecType = 0; //level of vec
    bool isPtr = type % 10;
    float type_deriv2 = type;
    

    int insertSymbPos = codedInsert.find('$');
    int insertIdx = -1;
    try {
        if(insertSymbPos != std::string::npos && insertSymbPos != 0) insertIdx = std::stoi(codedInsert.substr(0, insertSymbPos)) - padding;
        else insertIdx = -1;
    }
    catch(const std::exception& e) {
        insertIdx = -1;
    }
    if(insertIdx!=-1) codedInsert = codedInsert.substr(insertSymbPos+1);
    else codedInsert = "";


    if(type<100) {}
    else if(type<200) { type_deriv2 = static_cast<float>(type-100); vecType = 1; }
    else if(type<300) { type_deriv2 = static_cast<float>(type-200); vecType = 2; }
    

    pos = idx[pos];
    switch (static_cast<int>(floor(type_deriv2/10))) {
    case 0: //bool
        if(vecType==0 && isPtr) tempStr = std::string(padding,emptySpace) + bool_string(*values_0_bool_p[pos]) + std::string(padding,emptySpace);
        if(vecType==0 && !isPtr)tempStr = std::string(padding,emptySpace) + bool_string(values_0_bool[pos]) + std::string(padding,emptySpace);
        if(vecType==1 && isPtr) tempStr = prettyPrint_vec1<bool>(*values_1_bool_p[pos],align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==1 && !isPtr)tempStr = prettyPrint_vec1<bool>(values_1_bool[pos],   align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==2 && isPtr) tempStr = prettyPrint_vec2<bool>(*values_2_bool_p[pos],align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==2 && !isPtr)tempStr = prettyPrint_vec2<bool>(values_2_bool[pos],   align, decimals, width, padding, prettyPrint, left_indent);
        break;
    case 1: //int
        if(vecType==0 && isPtr) tempStr = std::string(padding,emptySpace) + dict_formatNumber<int>(*values_0_int_p[pos],width,decimals,align) + std::string(padding,emptySpace);
        if(vecType==0 && !isPtr)tempStr = std::string(padding,emptySpace) + dict_formatNumber<int>(values_0_int[pos],width,decimals,align) + std::string(padding,emptySpace);
        if(vecType==1 && isPtr) tempStr = prettyPrint_vec1<int>(*values_1_int_p[pos],  align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==1 && !isPtr)tempStr = prettyPrint_vec1<int>(values_1_int[pos],     align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==2 && isPtr) tempStr = prettyPrint_vec2<int>(*values_2_int_p[pos],  align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==2 && !isPtr)tempStr = prettyPrint_vec2<int>(values_2_int[pos],     align, decimals, width, padding, prettyPrint, left_indent);
        break;
    case 2: //float
        if(vecType==0 && isPtr) tempStr = std::string(padding,emptySpace) + dict_formatNumber<float>(*values_0_float_p[pos],width,decimals,align) + std::string(padding,emptySpace);
        if(vecType==0 && !isPtr)tempStr = std::string(padding,emptySpace) + dict_formatNumber<float>(values_0_float[pos],width,decimals,align) + std::string(padding,emptySpace);
        if(vecType==1 && isPtr) tempStr = prettyPrint_vec1<float>(*values_1_float_p[pos],  align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==1 && !isPtr)tempStr = prettyPrint_vec1<float>(values_1_float[pos],     align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==2 && isPtr) tempStr = prettyPrint_vec2<float>(*values_2_float_p[pos],  align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==2 && !isPtr)tempStr = prettyPrint_vec2<float>(values_2_float[pos],     align, decimals, width, padding, prettyPrint, left_indent);
        break;
    case 3: //double
        if(vecType==0 && isPtr) tempStr = std::string(padding,emptySpace) + dict_formatNumber<double>(*values_0_double_p[pos],width,decimals,align) + std::string(padding,emptySpace);
        if(vecType==0 && !isPtr)tempStr = std::string(padding,emptySpace) + dict_formatNumber<double>(values_0_double[pos],width,decimals,align) + std::string(padding,emptySpace);
        if(vecType==1 && isPtr) tempStr = prettyPrint_vec1<double>(*values_1_double_p[pos],align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==1 && !isPtr)tempStr = prettyPrint_vec1<double>(values_1_double[pos],   align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==2 && isPtr) tempStr = prettyPrint_vec2<double>(*values_2_double_p[pos],align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==2 && !isPtr)tempStr = prettyPrint_vec2<double>(values_2_double[pos],   align, decimals, width, padding, prettyPrint, left_indent);
        break;
    case 4: //char
        if(vecType==0 && isPtr) tempStr = std::string(padding,emptySpace) + dict_formatNumber<char>(*values_0_char_p[pos],width,decimals,align) + std::string(padding,emptySpace);
        if(vecType==0 && !isPtr)tempStr = std::string(padding,emptySpace) + dict_formatNumber<char>(values_0_char[pos],width,decimals,align) + std::string(padding,emptySpace);
        if(vecType==1 && isPtr) tempStr = prettyPrint_vec1<char>(*values_1_char_p[pos],align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==1 && !isPtr)tempStr = prettyPrint_vec1<char>(values_1_char[pos],   align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==2 && isPtr) tempStr = prettyPrint_vec2<char>(*values_2_char_p[pos],align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==2 && !isPtr)tempStr = prettyPrint_vec2<char>(values_2_char[pos],   align, decimals, width, padding, prettyPrint, left_indent);
        break;
    case 5: //std::string
        if(vecType==0 && isPtr) tempStr = std::string(padding,emptySpace) + dict_formatNumber<std::string>(*values_0_string_p[pos],width,decimals,align) + std::string(padding,emptySpace);
        if(vecType==0 && !isPtr)tempStr = std::string(padding,emptySpace) + dict_formatNumber<std::string>(values_0_string[pos],width,decimals,align) + std::string(padding,emptySpace);
        if(vecType==1 && isPtr) tempStr = prettyPrint_vec1<std::string>(*values_1_string_p[pos],align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==1 && !isPtr)tempStr = prettyPrint_vec1<std::string>(values_1_string[pos],   align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==2 && isPtr) tempStr = prettyPrint_vec2<std::string>(*values_2_string_p[pos],align, decimals, width, padding, prettyPrint, left_indent);
        if(vecType==2 && !isPtr)tempStr = prettyPrint_vec2<std::string>(values_2_string[pos],   align, decimals, width, padding, prettyPrint, left_indent);
        break;
    default:
        break;
    }

    std::string finalStr = "";
    if(insertIdx!=-1) finalStr = tempStr.substr(0,insertIdx)+codedInsert+tempStr.substr(insertIdx);
    else finalStr = tempStr;

    return finalStr;
}

/// @brief get the datatype code of `key` defined by `diy_dict::info_type_definition`
/// @param key to find the stored datatype code of
/// @return datatype code that's found. If `key` doesnt exist then it'll return `-1`
int diy_dict::get_type(std::string key) {
    int pos = check_existence(key);
    return datatype[pos];
}


int diy_dict::add(std::string key, bool value) {
    if(extend_reg(key, 0)==1) return 1;
    values_0_bool.push_back(value);
    idx.push_back(values_0_bool.size()-1);
    return 0;
}
int diy_dict::add(std::string key, int value) {
    if(extend_reg(key, 10)==1) return 1;
    values_0_int.push_back(value);
    idx.push_back(values_0_int.size()-1);
    return 0;
}
int diy_dict::add(std::string key, float value) {
    if(extend_reg(key, 20)==1) return 1;
    values_0_float.push_back(value);
    idx.push_back(values_0_float.size()-1);
    return 0;
}
int diy_dict::add(std::string key, double value) {
    if(extend_reg(key, 30)==1) return 1;
    values_0_double.push_back(value);
    idx.push_back(values_0_double.size()-1);
    return 0;
}
int diy_dict::add(std::string key, char value) {
    if(extend_reg(key, 40)==1) return 1;
    values_0_char.push_back(value);
    idx.push_back(values_0_char.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::string value) {
    if(extend_reg(key, 50)==1) return 1;
    values_0_string.push_back(value);
    idx.push_back(values_0_string.size()-1);
    return 0;
}

int diy_dict::add(std::string key, bool* ptr) {
    if(extend_reg(key, 1)==1) return 1;
    values_0_bool_p.push_back(ptr);
    idx.push_back(values_0_bool_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, int* ptr) {
    if(extend_reg(key, 11)==1) return 1;
    values_0_int_p.push_back(ptr);
    idx.push_back(values_0_int_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, float* ptr) {
    if(extend_reg(key, 21)==1) return 1;
    values_0_float_p.push_back(ptr);
    idx.push_back(values_0_float_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, double* ptr) {
    if(extend_reg(key, 31)==1) return 1;
    values_0_double_p.push_back(ptr);
    idx.push_back(values_0_double_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, char* ptr) {
    if(extend_reg(key, 41)==1) return 1;
    values_0_char_p.push_back(ptr);
    idx.push_back(values_0_char_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::string* ptr) {
    if(extend_reg(key, 51)==1) return 1;
    values_0_string_p.push_back(ptr);
    idx.push_back(values_0_string_p.size()-1);
    return 0;
}


int diy_dict::add(std::string key, std::vector<bool> value) {
    if(extend_reg(key, 100)==1) return 1;
    values_1_bool.push_back(value);
    idx.push_back(values_1_bool.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<int> value) {
    if(extend_reg(key, 110)==1) return 1;
    values_1_int.push_back(value);
    idx.push_back(values_1_int.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<float> value) {
    if(extend_reg(key, 120)==1) return 1;
    values_1_float.push_back(value);
    idx.push_back(values_1_float.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<double> value) {
    if(extend_reg(key, 130)==1) return 1;
    values_1_double.push_back(value);
    idx.push_back(values_1_double.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<char> value) {
    if(extend_reg(key, 140)==1) return 1;
    values_1_char.push_back(value);
    idx.push_back(values_1_char.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::string> value) {
    if(extend_reg(key, 150)==1) return 1;
    values_1_string.push_back(value);
    idx.push_back(values_1_string.size()-1);
    return 0;
}

int diy_dict::add(std::string key, std::vector<bool>* ptr) {
    if(extend_reg(key, 101)==1) return 1;
    values_1_bool_p.push_back(ptr);
    idx.push_back(values_1_bool_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<int>* ptr) {
    if(extend_reg(key, 111)==1) return 1;
    values_1_int_p.push_back(ptr);
    idx.push_back(values_1_int_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<float>* ptr) {
    if(extend_reg(key, 121)==1) return 1;
    values_1_float_p.push_back(ptr);
    idx.push_back(values_1_float_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<double>* ptr) {
    if(extend_reg(key, 131)==1) return 1;
    values_1_double_p.push_back(ptr);
    idx.push_back(values_1_double_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<char>* ptr) {
    if(extend_reg(key, 141)==1) return 1;
    values_1_char_p.push_back(ptr);
    idx.push_back(values_1_char_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::string>* ptr) {
    if(extend_reg(key, 151)==1) return 1;
    values_1_string_p.push_back(ptr);
    idx.push_back(values_1_string_p.size()-1);
    return 0;
}


int diy_dict::add(std::string key, std::vector<std::vector<bool>> value) {
    if(extend_reg(key, 200)==1) return 1;
    values_2_bool.push_back(value);
    idx.push_back(values_2_bool.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<int>> value) {
    if(extend_reg(key, 210)==1) return 1;
    values_2_int.push_back(value);
    idx.push_back(values_2_int.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<float>> value) {
    if(extend_reg(key, 220)==1) return 1;
    values_2_float.push_back(value);
    idx.push_back(values_2_float.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<double>> value) {
    if(extend_reg(key, 230)==1) return 1;
    values_2_double.push_back(value);
    idx.push_back(values_2_double.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<char>> value) {
    if(extend_reg(key, 240)==1) return 1;
    values_2_char.push_back(value);
    idx.push_back(values_2_char.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<std::string>> value) {
    if(extend_reg(key, 250)==1) return 1;
    values_2_string.push_back(value);
    idx.push_back(values_2_string.size()-1);
    return 0;
}

int diy_dict::add(std::string key, std::vector<std::vector<bool>>* ptr) {
    if(extend_reg(key, 201)==1) return 1;
    values_2_bool_p.push_back(ptr);
    idx.push_back(values_2_bool_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<int>>* ptr) {
    if(extend_reg(key, 211)==1) return 1;
    values_2_int_p.push_back(ptr);
    idx.push_back(values_2_int_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<float>>* ptr) {
    if(extend_reg(key, 221)==1) return 1;
    values_2_float_p.push_back(ptr);
    idx.push_back(values_2_float_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<double>>* ptr) {
    if(extend_reg(key, 231)==1) return 1;
    values_2_double_p.push_back(ptr);
    idx.push_back(values_2_double_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<char>>* ptr) {
    if(extend_reg(key, 241)==1) return 1;
    values_2_char_p.push_back(ptr);
    idx.push_back(values_2_char_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<std::string>>* ptr) {
    if(extend_reg(key, 251)==1) return 1;
    values_2_string_p.push_back(ptr);
    idx.push_back(values_2_string_p.size()-1);
    return 0;
}




int diy_dict::edit(std::string key, bool value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=  0) return 1;

    values_0_bool[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, int value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!= 10) return 1;

    values_0_int[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, float value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!= 20) return 1;

    values_0_float[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, double value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!= 30) return 1;

    values_0_double[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, char value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!= 40) return 1;

    values_0_char[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, std::string value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!= 50) return 1;

    values_0_string[idx[pos]] = value;
    return 0;
}

int diy_dict::edit(std::string key, bool* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=  1) return 1;

    values_0_bool_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, int* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!= 11) return 1;

    values_0_int_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, float* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!= 21) return 1;

    values_0_float_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, double* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!= 31) return 1;

    values_0_double_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, char* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!= 41) return 1;

    values_0_char_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, std::string* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!= 51) return 1;

    values_0_string_p[idx[pos]] = ptr;
    return 0;
}


int diy_dict::edit(std::string key, std::vector<bool> value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=100) return 1;

    values_1_bool[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<int> value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=110) return 1;

    values_1_int[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<float> value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=120) return 1;

    values_1_float[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<double> value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=130) return 1;

    values_1_double[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<char> value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=140) return 1;

    values_1_char[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<std::string> value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=150) return 1;

    values_1_string[idx[pos]] = value;
    return 0;
}

int diy_dict::edit(std::string key, std::vector<bool>* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=101) return 1;

    values_1_bool_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<int>* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=111) return 1;

    values_1_int_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<float>* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=121) return 1;

    values_1_float_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<double>* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=131) return 1;

    values_1_double_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<char>* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=141) return 1;

    values_1_char_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<std::string>* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=151) return 1;

    values_1_string_p[idx[pos]] = ptr;
    return 0;
}


int diy_dict::edit(std::string key, std::vector<std::vector<bool>> value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=200) return 1;

    values_2_bool[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<std::vector<int>> value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=210) return 1;

    values_2_int[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<std::vector<float>> value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=220) return 1;

    values_2_float[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<std::vector<double>> value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=230) return 1;

    values_2_double[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<std::vector<char>> value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=240) return 1;

    values_2_char[idx[pos]] = value;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<std::vector<std::string>> value) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=250) return 1;
    ;
    values_2_string[idx[pos]] = value;
    return 0;
}

int diy_dict::edit(std::string key, std::vector<std::vector<bool>>* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=201) return 1;

    values_2_bool_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<std::vector<int>>* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=211) return 1;

    values_2_int_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<std::vector<float>>* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=221) return 1;

    values_2_float_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<std::vector<double>>* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=231) return 1;

    values_2_double_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<std::vector<char>>* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=241) return 1;
    ;
    values_2_char_p[idx[pos]] = ptr;
    return 0;
}
int diy_dict::edit(std::string key, std::vector<std::vector<std::string>>* ptr) {
    int pos = check_existence(key);
    if(pos==-1) return 1;
    int type = datatype[pos];
    if(type!=251) return 1;

    values_2_string_p[idx[pos]] = ptr;
    return 0;
}




bool        diy_dict::get0_bool_  (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==  0) return values_0_bool.at(idx[pos]);
    return NULL;
}
int         diy_dict::get0_int_   (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]== 10) return values_0_int.at(idx[pos]);
    return NULL;
}
float       diy_dict::get0_float_ (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]== 20) return values_0_float.at(idx[pos]);
    return NULL;
}
double      diy_dict::get0_double_(std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]== 30) return values_0_double.at(idx[pos]);
    return NULL;
}
char        diy_dict::get0_char_  (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]== 40) return values_0_char.at(idx[pos]);
    return NULL;
}
std::string diy_dict::get0_string_(std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]== 50) return values_0_string.at(idx[pos]);
    return NULL;
}

bool*        diy_dict::get0_boolP  (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==  1) return values_0_bool_p.at(idx[pos]);
    return NULL;
}
int*         diy_dict::get0_intP   (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]== 11) return values_0_int_p.at(idx[pos]);
    return NULL;
}
float*       diy_dict::get0_floatP (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]== 21) return values_0_float_p.at(idx[pos]);
    return NULL;
}
double*      diy_dict::get0_doubleP(std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]== 31) return values_0_double_p.at(idx[pos]);
    return NULL;
}
char*        diy_dict::get0_charP  (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]== 41) return values_0_char_p.at(idx[pos]);
    return NULL;
}
std::string* diy_dict::get0_stringP(std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]== 51) return values_0_string_p.at(idx[pos]);
    return NULL;
}


std::vector<bool>           diy_dict::get1_bool_  (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==100) return values_1_bool.at(idx[pos]);
    return std::vector<bool>();
}
std::vector<int>            diy_dict::get1_int_   (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==110) return values_1_int.at(idx[pos]);
    return std::vector<int>();
}
std::vector<float>          diy_dict::get1_float_ (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==120) return values_1_float.at(idx[pos]);
    return std::vector<float>();
}
std::vector<double>         diy_dict::get1_double_(std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==130) return values_1_double.at(idx[pos]);
    return std::vector<double>();
}
std::vector<char>           diy_dict::get1_char_  (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==140) return values_1_char.at(idx[pos]);
    return std::vector<char>();
}
std::vector<std::string>    diy_dict::get1_string_(std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==150) return values_1_string.at(idx[pos]);
    return std::vector<std::string>();
}

std::vector<bool>*          diy_dict::get1_boolP  (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==101) return values_1_bool_p.at(idx[pos]);
    return nullptr;
}
std::vector<int>*           diy_dict::get1_intP   (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==111) return values_1_int_p.at(idx[pos]);
    return nullptr;
}
std::vector<float>*         diy_dict::get1_floatP (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==121) return values_1_float_p.at(idx[pos]);
    return nullptr;
}
std::vector<double>*        diy_dict::get1_doubleP(std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==131) return values_1_double_p.at(idx[pos]);
    return nullptr;
}
std::vector<char>*          diy_dict::get1_charP  (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==141) return values_1_char_p.at(idx[pos]);
    return nullptr;
}
std::vector<std::string>*   diy_dict::get1_stringP(std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==151) return values_1_string_p.at(idx[pos]);
    return nullptr;
}


std::vector<std::vector<bool>>        diy_dict::get2_bool_  (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==200) return values_2_bool.at(idx[pos]);
    return std::vector<std::vector<bool>>();
}
std::vector<std::vector<int>>         diy_dict::get2_int_   (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==210) return values_2_int.at(idx[pos]);
    return std::vector<std::vector<int>>();
}
std::vector<std::vector<float>>       diy_dict::get2_float_ (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==220) return values_2_float.at(idx[pos]);
    return std::vector<std::vector<float>>();
}
std::vector<std::vector<double>>      diy_dict::get2_double_(std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==230) return values_2_double.at(idx[pos]);
    return std::vector<std::vector<double>>();
}
std::vector<std::vector<char>>        diy_dict::get2_char_  (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==240) return values_2_char.at(idx[pos]);
    return std::vector<std::vector<char>>();
}
std::vector<std::vector<std::string>> diy_dict::get2_string_(std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==250) return values_2_string.at(idx[pos]);
    return std::vector<std::vector<std::string>>();
}

std::vector<std::vector<bool>>*        diy_dict::get2_boolP  (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==201) return values_2_bool_p.at(idx[pos]);
    return nullptr;
}
std::vector<std::vector<int>>*         diy_dict::get2_intP   (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==211) return values_2_int_p.at(idx[pos]);
    return nullptr;
}
std::vector<std::vector<float>>*       diy_dict::get2_floatP (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==221) return values_2_float_p.at(idx[pos]);
    return nullptr;
}
std::vector<std::vector<double>>*      diy_dict::get2_doubleP(std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==231) return values_2_double_p.at(idx[pos]);
    return nullptr;
}
std::vector<std::vector<char>>*        diy_dict::get2_charP  (std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==241) return values_2_char_p.at(idx[pos]);
    return nullptr;
}
std::vector<std::vector<std::string>>* diy_dict::get2_stringP(std::string key) {
    int pos = check_existence(key);
    if(pos!=-1 && datatype[pos]==251) return values_2_string_p.at(idx[pos]);
    return nullptr;
}

