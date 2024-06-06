
#include "diy_dictionary.hpp"




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
    if(pos[0]!=-1 && datatype[pos[0]]== 0) return values_0_bool.at(idx[pos[0]]);
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