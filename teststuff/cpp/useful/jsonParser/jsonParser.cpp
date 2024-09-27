
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

std::string const& JSON_P::jsonPair::toStr(
    int _styleOpt,
    int _indent,
    bool _only_value,
    int _width_key,
    int _width_val
) const {
    assert(_type!=-1);
    std::string out = "";

    bool val_LineB = false;
    if(_type==2 || _type==3) {
        switch (_styleOpt) {
        case 0: val_LineB = false; break;
        case 1:
            for(int _=0; _<2; _++) {
                const std::vector<JSON_P::jsonPair>& _ref = (_==0? _value_2 : _value_3);
                for(auto el: _ref) {
                    if(el.type()==2 || el.type()==3) { //check if there are any other arrays/object-literals in any of the "elements"/values
                        val_LineB = true;
                        break;
                    }
                }
                if(val_LineB) break;
            }
            break;
        case 2: val_LineB = true; break;
        case 3:
            if(_type==2) { val_LineB = true; break; }
            for(auto el: _value_3) {
                if(el.type()==2 || el.type()==3) { //check if there are any other arrays/object-literals in any of the "elements"/values
                    val_LineB = true;
                    break;
                }
            }
            break;
        default:
            break;
        }
    }


    out += std::string(_indent, ' ');
    if(!_onlyVal) {
        int keyLen = (_width_key<0?
            (_width_key==_WIDTH_AUTO? this->key.length() : 0) :
            this->key.length()
        );
        out += formatNumber("\""+this->key+"\"", keyLen+2, 0, "left") + ": ";
    }
    out += (_type==2? "{" : (_type==3? "[" : ""));
    if(val_LineB) {
        out += "\n";// + std::string(_indent, ' ');
    }

    int len1_key    = 0;
    int len1_val    = 0;
    if(_width_val<0) {
        switch (_width_val) {
        case _WIDTH_MAX: break;
        case _WIDTH_MAX_KEY:
            if(_type==2) {
                std::vector<size_t> key1Len{0};

                for(auto el: _value_2) key1Len.push_back(el.key.length());
                len1_key = findVal<size_t>(key1Len, 0);
            }
            break;
        }
    }

    if(_type==2 || _type==3) {
        if(const_cast<JSON_P::jsonPair&>((_type==2? _value_2 : _value_3)[0]).type()==4) {
            // if(_width_val==_WIDTH_MAX)
            len1_val = 5; //"false"
        }
    }

    switch (this->_type) {
        case 0:     out += "\""+_value_0+"\","; break;
        case 10:    out += formatNumber(_value_10, _width_val, 0); break;
        case 11:    out += formatNumber(_value_11, _width_val, _str_decimal_precision); break;
        case 12:    out += formatNumber(_value_12, _width_val, _str_decimal_precision); break;
        case 2:
            for(size_t i=0; i<_value_2.size(); i++) {
                if(val_LineB) out += std::string(_indent, ' ');
                out += _value_2[i].toStr(_styleOpt, _indent, _only_value, len1_key, len1_val);

                if(i<_value_2.size()-1) out += ",";
                if(val_LineB) out += "\n";
            }
            break;
        case 3:
            for(size_t i=0; i<_value_3.size(); i++) {
                if(val_LineB) out += std::string(_indent, ' ');
                out += _value_3[i].toStr(_styleOpt, _indent, _only_value, len1_key, len1_val);

                if(i<_value_3.size()-1) out += ",";
                if(val_LineB) out += "\n";
            }
            break;
        case 4:     out += formatNumber(std::string((_value_4? "true" : "false")), _width_val); break;
        default:
            break;
    }

    if(val_LineB) out += std::string(_indent, ' ');
    out += (_type==2? "}" : (_type==3? "]" : ""));


    this->_toStr = out;

    return out;
}

JSON_P::jsonPair& JSON_P::jsonPair::operator[] (std::string _key) {
    assert(_type==2);

    for(JSON_P::jsonPair& elem: _value_2) {
        if(elem.key==_key) return elem;
    }
    std::runtime_error("ERROR: "+_info_name+"::operator[](std::string):  the key \""+_key+"\" doesn't exist in json object");
}
JSON_P::jsonPair& JSON_P::jsonPair::operator[] (int _idx) {
    assert(_type==2 || _type==3);

    std::vector<JSON_P::jsonPair>& toReturn = (_type==2? _value_2 : _value_3);

    int valSize = toReturn.size();
    assert(_idx<valSize);
    if(_idx<0) {
        assert(abs(_idx)<=valSize);
        _idx = static_cast<int>(valSize) + _idx;
    }

    return toReturn[_idx];
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

