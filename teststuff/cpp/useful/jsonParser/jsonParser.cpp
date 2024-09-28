
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


std::string const& JSON_P::jsonPair::toStr(
    int     _styleOpt,
    int     _indent,
    bool    _only_value,
    int     _width_key,
    int     _width_val,
    bool    _no_key,
    bool    _indent_pair
) const {

    //check if there are any other arrays/object-literals in any of the "elements"/values
    static auto hasCont = [](const std::vector<jsonPair>& _vec) {
        for(auto el: _vec) {
            if(el.type()==2 || el.type()==3) return true;
        }
        return false;
    };


    assert(_type!=-1);
    std::string out = "";

    bool _verbose = false;

    if(_verbose) std::cout << "-------start: _type: " << _type << std::endl;

    bool val_LineB  = false;
    bool indentSub  = false;
    // bool val_LineB1 = false;
    if(_type==2 || _type==3) {
        switch (_styleOpt) {
            case 0: val_LineB = false; break;
            case 1:
                for(int _=0; _<2; _++) {
                    const std::vector<JSON_P::jsonPair>& _ref = (_==0? _value_2 : _value_3);
                    if(hasCont(_ref)) {
                        val_LineB = true;
                        indentSub = true;
                        break;
                    }
                }
                break;
            case 2: val_LineB = true; indentSub = true; break;
            case 3:
                if(_type==2) { val_LineB = true; indentSub = true; break; }
                if(hasCont(_value_3)) {
                    val_LineB = true;
                    indentSub = true;
                }
                break;
        }
    }

    if(_verbose) std::cout << formatNumber("val_LineB",10,0,"left")<<":" << std::boolalpha << val_LineB << std::endl;

    if(_indent_pair) out += std::string(_indent, ' ');
    if(!_no_key) {
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

    if(_verbose) std::cout << formatNumber("out: ",10,0,"left")<<": [\"" << out << "\"]" << std::endl;

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
    else {
        if(_verbose) {
            std::cout << formatNumber("value",10,0,"left")<<": ";
            switch (_type) {
                case  0: std::cout<<_value_0;  break;
                case 10: std::cout<<_value_10; break;
                case 11: std::cout<<_value_11; break;
                case 12: std::cout<<_value_12; break;
                case  4: std::cout<<std::boolalpha<<_value_4;  break;
            }
            std::cout << std::endl;
        }    
    }

    std::cout << "_type: "<<_type<<": " << std::boolalpha << val_LineB << std::endl;
    switch (this->_type) {
        case 0:     out += "\""+_value_0+"\""; break;
        case 10:    out += formatNumber(_value_10, _width_val, 0); break;
        case 11:    out += formatNumber(_value_11, _width_val, _str_decimal_precision); break;
        case 12:    out += formatNumber(_value_12, _width_val, _str_decimal_precision); break;
        case 2:
            for(size_t i=0; i<_value_2.size(); i++) {
                std::string _temp = _value_2[i].toStr(_styleOpt, _indent, _only_value, len1_key, len1_val, false, indentSub);
                if(_verbose) std::cout << formatNumber("_temp",10,0,"left")<<": \"" <<_temp << "\"" << std::endl;
                
                if(val_LineB) {
                    out += std::string(_indent, ' ');
                    for(size_t ii=0; ii<_temp.length(); ii++) {
                        if(_temp[ii]=='\n') {
                                _temp.insert(ii+1, std::string(_indent, ' '));
                                ii += _indent*2;
                            
                        }
                    }
                }
                out += _temp;

                if(i<_value_2.size()-1) out += ",";
                if(val_LineB) { out += "\n"; }
                else if(i<_value_2.size()-1) out += " ";
            }
            break;
        case 3:
            for(size_t i=0; i<_value_3.size(); i++) {
                
                std::string _temp = _value_3[i].toStr(_styleOpt, _indent, _only_value, len1_key, len1_val, true, indentSub);
                
                if(val_LineB) {
                    out += std::string(_indent, ' ');
                    for(size_t ii=0; ii<_temp.length(); ii++) {
                        if(_temp[ii]=='\n') {
                            _temp.insert(ii+1, std::string(_indent, ' '));
                            ii += _indent*2;
                        }
                    }
                }
                out += _temp;

                if(i<_value_3.size()-1) out += ",";
                if(val_LineB) out += "\n";
                else if(i<_value_3.size()-1) out += " ";
            }
            break;
        case 4:     out += formatNumber(std::string((_value_4? "true" : "false")), _width_val); break;
        default:
            break;
    }

    if(val_LineB) out += std::string(_indent, ' ');
    out += (_type==2? "}" : (_type==3? "]" : ""));


    this->_toStr = out;

    if(_verbose) std::cout << "---------------"<< formatNumber("_type: ",10,0,"left")<<": "<<_type << std::endl;

    return _toStr;
}


std::ostream& JSON_P::operator<<(std::ostream& os, const JSON_P::jsonPair& jP) {
    return os << jP.toStr();
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

JSON_P::jsonPair& JSON_P::jsonPair::operator= (std::string _newVal) {
    switch (_type) {
        case 2: _value_2.clear(); break;
        case 3: _value_3.clear(); break;
    }
    _type = 0;
    _value_0 = _newVal;
    return *this;
}
JSON_P::jsonPair& JSON_P::jsonPair::operator= (int _newVal) {
    switch (_type) {
        case 2: _value_2.clear(); break;
        case 3: _value_3.clear(); break;
    }
    _type = 10;
    _value_10 = _newVal;
    return *this;
}
JSON_P::jsonPair& JSON_P::jsonPair::operator= (float _newVal) {
    switch (_type) {
        case 2: _value_2.clear(); break;
        case 3: _value_3.clear(); break;
    }
    _type = 11;
    _value_10 = _newVal;
    return *this;
}
JSON_P::jsonPair& JSON_P::jsonPair::operator= (double _newVal) {
    switch (_type) {
        case 2: _value_2.clear(); break;
        case 3: _value_3.clear(); break;
    }
    _type = 12;
    _value_12 = _newVal;
    return *this;
}
JSON_P::jsonPair& JSON_P::jsonPair::operator= (std::vector<JSON_P::jsonPair> _newVal) {
    assert(_newVal.size()>0);

    int _vecType = _newVal[0].type();

    if(_vecType==_type) {
        if(_vecType==2) _value_2 = _newVal;
        else _value_3 = _newVal;
    }
    else {
        switch (_type) {
            case 2: _value_2.clear(); break;
            case 3: _value_3.clear(); break;
        }
        _type = _vecType;
        if(_vecType==2) _value_2 = _newVal;
        else _value_3 = _newVal;
    }
    return *this;
}
JSON_P::jsonPair& JSON_P::jsonPair::operator= (bool _newVal) {
    switch (_type) {
        case 2: _value_2.clear(); break;
        case 3: _value_3.clear(); break;
    }
    _type = 4;
    _value_4 = _newVal;
    return *this;
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




// JSON_P::Parser::Parser() {

// }
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

