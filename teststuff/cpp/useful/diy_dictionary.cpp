
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
int diy_dict::_extend_reg(std::string key, int varType) {
    if(check_existence(key) != -1) return 1;
    keys.push_back(key);
    datatype.push_back(varType);
    return 0;
}

int diy_dict::_erase_idx(int typeID, int idx) {
    if(typeID<this->typeLIM[0] || typeID>this->typeLIM[1]) return -1;

    switch (typeID) {
        case   0: values_0_bool.erase(this->values_0_bool.begin()       +idx); break;
        case  10: values_0_int.erase(this->values_0_int.begin()         +idx); break;
        case  20: values_0_float.erase(this->values_0_float.begin()     +idx); break;
        case  30: values_0_double.erase(this->values_0_double.begin()   +idx); break;
        case  40: values_0_char.erase(this->values_0_char.begin()       +idx); break;
        case  50: values_0_string.erase(this->values_0_string.begin()   +idx); break;
        case   1: values_0_bool_p.erase(this->values_0_bool_p.begin()       +idx); break;
        case  11: values_0_int_p.erase(this->values_0_int_p.begin()         +idx); break;
        case  21: values_0_float_p.erase(this->values_0_float_p.begin()     +idx); break;
        case  31: values_0_double_p.erase(this->values_0_double_p.begin()   +idx); break;
        case  41: values_0_char_p.erase(this->values_0_char_p.begin()       +idx); break;
        case  51: values_0_string_p.erase(this->values_0_string_p.begin()   +idx); break;
        case 100: values_1_bool.erase(this->values_1_bool.begin()       +idx); break;
        case 110: values_1_int.erase(this->values_1_int.begin()         +idx); break;
        case 120: values_1_float.erase(this->values_1_float.begin()     +idx); break;
        case 130: values_1_double.erase(this->values_1_double.begin()   +idx); break;
        case 140: values_1_char.erase(this->values_1_char.begin()       +idx); break;
        case 150: values_1_string.erase(this->values_1_string.begin()   +idx); break;
        case 101: values_1_bool_p.erase(this->values_1_bool_p.begin()       +idx); break;
        case 111: values_1_int_p.erase(this->values_1_int_p.begin()         +idx); break;
        case 121: values_1_float_p.erase(this->values_1_float_p.begin()     +idx); break;
        case 131: values_1_double_p.erase(this->values_1_double_p.begin()   +idx); break;
        case 141: values_1_char_p.erase(this->values_1_char_p.begin()       +idx); break;
        case 151: values_1_string_p.erase(this->values_1_string_p.begin()   +idx); break;
        case 200: values_2_bool.erase(this->values_2_bool.begin()       +idx); break;
        case 210: values_2_int.erase(this->values_2_int.begin()         +idx); break;
        case 220: values_2_float.erase(this->values_2_float.begin()     +idx); break;
        case 230: values_2_double.erase(this->values_2_double.begin()   +idx); break;
        case 240: values_2_char.erase(this->values_2_char.begin()       +idx); break;
        case 250: values_2_string.erase(this->values_2_string.begin()   +idx); break;
        case 201: values_2_bool_p.erase(this->values_2_bool_p.begin()   +idx); break;
        case 211: values_2_int_p.erase(this->values_2_int_p.begin()         +idx); break;
        case 221: values_2_float_p.erase(this->values_2_float_p.begin()     +idx); break;
        case 231: values_2_double_p.erase(this->values_2_double_p.begin()   +idx); break;
        case 241: values_2_char_p.erase(this->values_2_char_p.begin()       +idx); break;
        case 251: values_2_string_p.erase(this->values_2_string_p.begin()   +idx); break;
        default:
            return 1;
        break;
    }
    return 0;
}


std::string diy_dict::_bool_string(bool boolVar) {
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
        if(vecType==0 && isPtr) tempStr = std::string(padding,emptySpace) + _bool_string(*values_0_bool_p[pos]) + std::string(padding,emptySpace);
        if(vecType==0 && !isPtr)tempStr = std::string(padding,emptySpace) + _bool_string(values_0_bool[pos]) + std::string(padding,emptySpace);
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

/**
 * @brief Get the corresponding size of a dedicated container to a typeID/type-code
 * 
 * @param typeID the "code"/"ID" of the container to return the size of
 * @return `int` value of size of container if the type code container is successfully found. If the container of `typeID` was not found then `-1` is returned.
 */
int diy_dict::get_type_size(int typeID) {
    int vecSize = 0;
    if(typeID<this->typeLIM[0] || typeID>this->typeLIM[1]) return -1;

    switch (typeID) {
    case   0: vecSize = static_cast<int>(values_0_bool.size());     break;
    case  10: vecSize = static_cast<int>(values_0_int.size());      break;
    case  20: vecSize = static_cast<int>(values_0_float.size());    break;
    case  30: vecSize = static_cast<int>(values_0_double.size());   break;
    case  40: vecSize = static_cast<int>(values_0_char.size());     break;
    case  50: vecSize = static_cast<int>(values_0_string.size());   break;
    case   1: vecSize = static_cast<int>(values_0_bool_p.size());   break;
    case  11: vecSize = static_cast<int>(values_0_int_p.size());    break;
    case  21: vecSize = static_cast<int>(values_0_float_p.size());  break;
    case  31: vecSize = static_cast<int>(values_0_double_p.size()); break;
    case  41: vecSize = static_cast<int>(values_0_char_p.size());   break;
    case  51: vecSize = static_cast<int>(values_0_string_p.size()); break;
    case 100: vecSize = static_cast<int>(values_1_bool.size());     break;
    case 110: vecSize = static_cast<int>(values_1_int.size());      break;
    case 120: vecSize = static_cast<int>(values_1_float.size());    break;
    case 130: vecSize = static_cast<int>(values_1_double.size());   break;
    case 140: vecSize = static_cast<int>(values_1_char.size());     break;
    case 150: vecSize = static_cast<int>(values_1_string.size());   break;
    case 101: vecSize = static_cast<int>(values_1_bool_p.size());   break;
    case 111: vecSize = static_cast<int>(values_1_int_p.size());    break;
    case 121: vecSize = static_cast<int>(values_1_float_p.size());  break;
    case 131: vecSize = static_cast<int>(values_1_double_p.size()); break;
    case 141: vecSize = static_cast<int>(values_1_char_p.size());   break;
    case 151: vecSize = static_cast<int>(values_1_string_p.size()); break;
    case 200: vecSize = static_cast<int>(values_2_bool.size());     break;
    case 210: vecSize = static_cast<int>(values_2_int.size());      break;
    case 220: vecSize = static_cast<int>(values_2_float.size());    break;
    case 230: vecSize = static_cast<int>(values_2_double.size());   break;
    case 240: vecSize = static_cast<int>(values_2_char.size());     break;
    case 250: vecSize = static_cast<int>(values_2_string.size());   break;
    case 201: vecSize = static_cast<int>(values_2_bool_p.size());   break;
    case 211: vecSize = static_cast<int>(values_2_int_p.size());    break;
    case 221: vecSize = static_cast<int>(values_2_float_p.size());  break;
    case 231: vecSize = static_cast<int>(values_2_double_p.size()); break;
    case 241: vecSize = static_cast<int>(values_2_char_p.size());   break;
    case 251: vecSize = static_cast<int>(values_2_string_p.size()); break;
    default:
        return -1;
        break;
    }

    return vecSize;
}
/**
 * @brief Get the size of corresponding container size to a typeID/type-code used by the stored value/ptr assigned with `key`.
 * TL;DR how many of type (same as `key`'s stoed type) is stored in this dict.
 * @param key the `std::string` key whose container size is to be returned
 * @return `int` value of size of container if the type code container is successfully found. If the container of `key` was not found then `-1` is returned.
 */
int diy_dict::get_type_size(std::string key) {
    int pos = this->check_existence(key);
    int typeCode = datatype[pos];
    if(pos==-1) return -1;
    int vecSize = this->get_type_size(typeCode);
    return vecSize;
}


/**
 * @brief rename key of a stored index
 * 
 * @param key the key to change
 * @param new_key the new key
 * @return `int` of whether a key was successfully renamed. `0` - successful. `-1` - error-occurred/given-`key`-doesn't-exist
 */
int diy_dict::rename_key(std::string key, std::string new_key) {
    int pos = this->check_existence(key);
    if(pos==-1) return 1;
    keys[idx[pos]] = new_key;

    return 0;
}


/**
 * @brief Remove/Delete `key` and it's stored value/ptr from the dictionary
 * 
 * @param key the key to delete
 * @return whether it successfully deleted. `0`-successful; `1`-error occurred.
 */
int diy_dict::delete_key(std::string key) {
    /**
     * need to change containers:
     * - `keys`     {change: erase(key)}
     * - `datatype` {change: erase(key)}
     * - `{relative}
     * - `idx`      {change: erase(key), {-1 all same-container elements above pos}}
     */

    int pos = this->check_existence(key); //position/index in "navigator" vectors
    if(pos==-1) return 1;
    int local_idx = idx[pos]; //position/index in "storage" vectors
    int typeID = datatype[pos]; //typeID/type-code to what "storage" vector is related to 'key`
    int local_size = this->get_type_size(typeID); //size of "storage" vector related to `key`


    // erase the stored value/pointer of `key` in corresponding "storage" vector
    this->_erase_idx(typeID, local_idx);


    // get all occurrences of typeID
    std::vector<int> typeOcc = DIY_SEARCH_MULTITHREAD::multithread_searchVec<int>(this->datatype, typeID, 1, -1, true);
    if(typeOcc[0]==-1) return 1;


    // subtract -1 of all idx[pos] elements, with same "storage" container type as `key`, that has its value/pointer stored "further from 0" than `key`
    for(int elem_idx: typeOcc) {
        if(elem_idx>pos) this->idx[elem_idx]--;
    }


    // erase all "navigator" elements related to `key`.
    this->keys.erase(this->keys.begin()+pos);
    this->datatype.erase(this->datatype.begin()+pos);
    this->idx.erase(this->idx.begin()+pos);


    return 0;
}



int diy_dict::add(std::string key, bool value) {
    if(this->_extend_reg(key, 0)==1) return 1;
    values_0_bool.push_back(value);
    idx.push_back(values_0_bool.size()-1);
    return 0;
}
int diy_dict::add(std::string key, int value) {
    if(this->_extend_reg(key, 10)==1) return 1;
    values_0_int.push_back(value);
    idx.push_back(values_0_int.size()-1);
    return 0;
}
int diy_dict::add(std::string key, float value) {
    if(this->_extend_reg(key, 20)==1) return 1;
    values_0_float.push_back(value);
    idx.push_back(values_0_float.size()-1);
    return 0;
}
int diy_dict::add(std::string key, double value) {
    if(this->_extend_reg(key, 30)==1) return 1;
    values_0_double.push_back(value);
    idx.push_back(values_0_double.size()-1);
    return 0;
}
int diy_dict::add(std::string key, char value) {
    if(this->_extend_reg(key, 40)==1) return 1;
    values_0_char.push_back(value);
    idx.push_back(values_0_char.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::string value) {
    if(this->_extend_reg(key, 50)==1) return 1;
    values_0_string.push_back(value);
    idx.push_back(values_0_string.size()-1);
    return 0;
}

int diy_dict::add(std::string key, bool* ptr) {
    if(this->_extend_reg(key, 1)==1) return 1;
    values_0_bool_p.push_back(ptr);
    idx.push_back(values_0_bool_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, int* ptr) {
    if(this->_extend_reg(key, 11)==1) return 1;
    values_0_int_p.push_back(ptr);
    idx.push_back(values_0_int_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, float* ptr) {
    if(this->_extend_reg(key, 21)==1) return 1;
    values_0_float_p.push_back(ptr);
    idx.push_back(values_0_float_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, double* ptr) {
    if(this->_extend_reg(key, 31)==1) return 1;
    values_0_double_p.push_back(ptr);
    idx.push_back(values_0_double_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, char* ptr) {
    if(this->_extend_reg(key, 41)==1) return 1;
    values_0_char_p.push_back(ptr);
    idx.push_back(values_0_char_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::string* ptr) {
    if(this->_extend_reg(key, 51)==1) return 1;
    values_0_string_p.push_back(ptr);
    idx.push_back(values_0_string_p.size()-1);
    return 0;
}


int diy_dict::add(std::string key, std::vector<bool> value) {
    if(this->_extend_reg(key, 100)==1) return 1;
    values_1_bool.push_back(value);
    idx.push_back(values_1_bool.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<int> value) {
    if(this->_extend_reg(key, 110)==1) return 1;
    values_1_int.push_back(value);
    idx.push_back(values_1_int.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<float> value) {
    if(this->_extend_reg(key, 120)==1) return 1;
    values_1_float.push_back(value);
    idx.push_back(values_1_float.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<double> value) {
    if(this->_extend_reg(key, 130)==1) return 1;
    values_1_double.push_back(value);
    idx.push_back(values_1_double.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<char> value) {
    if(this->_extend_reg(key, 140)==1) return 1;
    values_1_char.push_back(value);
    idx.push_back(values_1_char.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::string> value) {
    if(this->_extend_reg(key, 150)==1) return 1;
    values_1_string.push_back(value);
    idx.push_back(values_1_string.size()-1);
    return 0;
}

int diy_dict::add(std::string key, std::vector<bool>* ptr) {
    if(this->_extend_reg(key, 101)==1) return 1;
    values_1_bool_p.push_back(ptr);
    idx.push_back(values_1_bool_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<int>* ptr) {
    if(this->_extend_reg(key, 111)==1) return 1;
    values_1_int_p.push_back(ptr);
    idx.push_back(values_1_int_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<float>* ptr) {
    if(this->_extend_reg(key, 121)==1) return 1;
    values_1_float_p.push_back(ptr);
    idx.push_back(values_1_float_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<double>* ptr) {
    if(this->_extend_reg(key, 131)==1) return 1;
    values_1_double_p.push_back(ptr);
    idx.push_back(values_1_double_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<char>* ptr) {
    if(this->_extend_reg(key, 141)==1) return 1;
    values_1_char_p.push_back(ptr);
    idx.push_back(values_1_char_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::string>* ptr) {
    if(this->_extend_reg(key, 151)==1) return 1;
    values_1_string_p.push_back(ptr);
    idx.push_back(values_1_string_p.size()-1);
    return 0;
}


int diy_dict::add(std::string key, std::vector<std::vector<bool>> value) {
    if(this->_extend_reg(key, 200)==1) return 1;
    values_2_bool.push_back(value);
    idx.push_back(values_2_bool.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<int>> value) {
    if(this->_extend_reg(key, 210)==1) return 1;
    values_2_int.push_back(value);
    idx.push_back(values_2_int.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<float>> value) {
    if(this->_extend_reg(key, 220)==1) return 1;
    values_2_float.push_back(value);
    idx.push_back(values_2_float.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<double>> value) {
    if(this->_extend_reg(key, 230)==1) return 1;
    values_2_double.push_back(value);
    idx.push_back(values_2_double.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<char>> value) {
    if(this->_extend_reg(key, 240)==1) return 1;
    values_2_char.push_back(value);
    idx.push_back(values_2_char.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<std::string>> value) {
    if(this->_extend_reg(key, 250)==1) return 1;
    values_2_string.push_back(value);
    idx.push_back(values_2_string.size()-1);
    return 0;
}

int diy_dict::add(std::string key, std::vector<std::vector<bool>>* ptr) {
    if(this->_extend_reg(key, 201)==1) return 1;
    values_2_bool_p.push_back(ptr);
    idx.push_back(values_2_bool_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<int>>* ptr) {
    if(this->_extend_reg(key, 211)==1) return 1;
    values_2_int_p.push_back(ptr);
    idx.push_back(values_2_int_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<float>>* ptr) {
    if(this->_extend_reg(key, 221)==1) return 1;
    values_2_float_p.push_back(ptr);
    idx.push_back(values_2_float_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<double>>* ptr) {
    if(this->_extend_reg(key, 231)==1) return 1;
    values_2_double_p.push_back(ptr);
    idx.push_back(values_2_double_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<char>>* ptr) {
    if(this->_extend_reg(key, 241)==1) return 1;
    values_2_char_p.push_back(ptr);
    idx.push_back(values_2_char_p.size()-1);
    return 0;
}
int diy_dict::add(std::string key, std::vector<std::vector<std::string>>* ptr) {
    if(this->_extend_reg(key, 251)==1) return 1;
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

