
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
