
#include "jsonParser.hpp"




JSON_P::jsonPair::jsonPair(
    std::string _key,
    std::string _value
): key(_key), _type(0) {
    _value_0    = _value;
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    int _value
): key(_key), _type(10) {
    _value_10   = _value;
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    float _value
): key(_key), _type(11) {
    _value_11   = _value;
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    double _value
): key(_key), _type(12) {
    _value_12   = _value;
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    std::initializer_list<jsonPair> _value,
    bool isArray
): key(_key), _type((isArray? 3 : 2)) {
    if(isArray) {
        _value_3    = _value;
        _onlyVal    = true;
    }
    else {
        _value_2    = _value;
    }
    
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    std::vector<jsonPair> _value,
    bool isArray
): key(_key), _type((isArray? 3 : 2)) {
    if(isArray) {
        _value_3    = _value;
        _onlyVal    = true;
    }
    else {
        _value_2    = _value;
    }
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    bool _value
): key(_key), _type(4) {
    _value_4     = _value;
}

const int JSON_P::jsonPair::type() {
    return this->_type;
}

std::ostream& JSON_P::operator<<(std::ostream& os, const JSON_P::jsonPair& jP) {
    return os << jP.toStr();
}

std::string const& JSON_P::jsonPair::toStr() const {
    std::string out;



    return out;
}

std::string& JSON_P::jsonPair::get0() {
    assert(_type==0);
    return this->_value_0;
}
int& JSON_P::jsonPair::get10() {
    assert(_type==10);
    return this->_value_10;
}
float& JSON_P::jsonPair::get11() {
    assert(_type==11);
    return this->_value_11;
}
double& JSON_P::jsonPair::get12() {
    assert(_type==12);
    return this->_value_12;
}
std::vector<JSON_P::jsonPair>& JSON_P::jsonPair::get2() {
    assert(_type==2);
    return this->_value_2;
}
std::vector<JSON_P::jsonPair>& JSON_P::jsonPair::get3() {
    assert(_type==3);
    return this->_value_3;
}
bool& JSON_P::jsonPair::get4() {
    assert(_type==4);
    return this->_value_4;
}




JSON_P::Parser::Parser() {

}
JSON_P::Parser::Parser(
    std::string filename,
    bool _verbose
) {

}



void JSON_P::Parser::loadFile(
    std::string filename,
    bool _verbose
) {

}

