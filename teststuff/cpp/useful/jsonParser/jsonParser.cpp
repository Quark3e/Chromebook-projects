
#include "jsonParser.hpp"


bool JSON_P::jsonPair::init() const {
    return this->_init;
}

JSON_P::jsonPair* JSON_P::jsonPair::getParent() {
    return this->_parent;
}
int JSON_P::jsonPair::getSiblingIdx() {
    return this->_siblingIndex;
}

void JSON_P::jsonPair::setParent(jsonPair* parent) {
    assert(parent);
    this->_parent = parent;
}
void JSON_P::jsonPair::setSiblingIndex(int idx) {
    assert(idx>0);
    this->_siblingIndex = idx;
}

void JSON_P::jsonPair::isArray(bool isArray) {
    assert(_type==2 || _type==3);
    
    _type = (isArray? 3 : 2);
}

JSON_P::jsonPair::jsonPair(
    std::string _key,
    JSON_P::jsonPair* parent
): _init(true), _parent(parent), key(_key) {


}

JSON_P::jsonPair::jsonPair(
    std::string _key,
    std::string _value,
    bool onlyVal,
    JSON_P::jsonPair* parent
): _init(true), _parent(parent), key(_key), _onlyVal(onlyVal), _type(0) {
    _value_0    = _value;
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    int _value,
    bool onlyVal,
    JSON_P::jsonPair* parent
): _init(true), _parent(parent), key(_key), _onlyVal(onlyVal), _type(10) {
    _value_10   = _value;
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    float _value,
    bool onlyVal,
    JSON_P::jsonPair* parent
): _init(true), _parent(parent), key(_key), _onlyVal(onlyVal), _type(11) {
    _value_11   = _value;
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    double _value,
    bool onlyVal,
    JSON_P::jsonPair* parent
): _init(true), _parent(parent), key(_key), _onlyVal(onlyVal), _type(12) {
    _value_12   = _value;
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    std::initializer_list<jsonPair> _value,
    bool isArray,
    JSON_P::jsonPair* parent
): _init(true), _parent(parent), key(_key), _type((isArray? 3 : 2)) {
    if(isArray) {
        _value_3    = _value;
        _onlyVal    = true;
        for(jsonPair& _pair : _value_3) _pair.setParent(this);
    }
    else {
        _value_2    = _value;
        for(jsonPair& _pair : _value_2) _pair.setParent(this);
    }
    
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    std::vector<jsonPair> _value,
    bool isArray,
    JSON_P::jsonPair* parent
): _init(true), _parent(parent), key(_key), _type((isArray? 3 : 2)) {
    if(isArray) {
        _value_3    = std::list<jsonPair>(_value.begin(), _value.end());
        _onlyVal    = true;
        for(jsonPair& _pair : _value_3) _pair.setParent(this);
    }
    else {
        _value_2    = std::list<jsonPair>(_value.begin(), _value.end());
        for(jsonPair& _pair : _value_2) _pair.setParent(this);
    }
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    std::list<jsonPair> _value,
    bool isArray,
    JSON_P::jsonPair* parent
): _init(true), _parent(parent), key(_key), _type((isArray? 3 : 2)) {
    if(isArray) {
        _value_3    = _value;
        _onlyVal    = true;
        for(jsonPair& _pair : _value_3) _pair.setParent(this);
    }
    else {
        _value_2    = _value;
        for(jsonPair& _pair : _value_2) _pair.setParent(this);
    }
}
JSON_P::jsonPair::jsonPair(
    std::string _key,
    bool _value,
    bool onlyVal,
    JSON_P::jsonPair* parent
): _init(true), _parent(parent), key(_key), _onlyVal(onlyVal), _type(4) {
    _value_4     = _value;
}

void JSON_P::jsonPair::append(jsonPair newPair) {
    assert(_type==2 || _type==3);
    this->insert(-1, newPair);

}
void JSON_P::jsonPair::insert(int idx, jsonPair newPair) {
    assert(_type==2 || _type==3);

    std::list<jsonPair>& lePair = (_type==2? _value_2 : _value_3);
    int _Lsize = static_cast<int>(lePair.size());
    assert(idx < _Lsize);
    if(idx<0) {
        if(idx!=-1) assert(abs(idx) <= _Lsize);
        idx = _Lsize += idx;
    }
    auto itr = lePair.begin();
    std::advance(itr, idx);
    lePair.insert(itr, newPair);
    lePair.back().setParent(this);
}
void JSON_P::jsonPair::erase(int idx) {
    assert(_type==2 || _type==3);

    std::list<jsonPair>& lePair = (_type==2? _value_2 : _value_3);
    int _Lsize = static_cast<int>(lePair.size());
    assert(idx < _Lsize);
    if(idx<0) {
        assert(abs(idx) <= _Lsize);
        idx = _Lsize += idx;
    }
    auto itr = lePair.begin();
    std::advance(itr, idx);
    lePair.erase(itr);
}

const int JSON_P::jsonPair::type() {
    return this->_type;
}

const size_t JSON_P::jsonPair::size() {
    switch (_type) {
    case 2: return _value_2.size(); break;
    case 3: return _value_3.size(); break;
    default:
        assert(false);
        break;
    }
    return 0;
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
    static auto hasCont = [](const std::list<jsonPair>& _vec) {
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
                    const std::list<JSON_P::jsonPair>& _ref = (_==0? _value_2 : _value_3);
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
    if(!(_no_key || _onlyVal)) {
        int keyLen = (_width_key<0?
            (_width_key==_WIDTH_AUTO? this->key.length() : 0) :
            _width_key
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
    // if(_width_val<0) {
    //     switch (_width_val) {
    //     case _WIDTH_MAX: break;
    //     case _WIDTH_MAX_KEY:
            if(_type==2) {
                std::vector<size_t> key1Len{0};

                for(auto el: _value_2) key1Len.push_back(el.key.length());
                len1_key = findVal<size_t>(key1Len, 0);
            }
    //         break;
    //     }
    // }

    if(_type==2 || _type==3) {
        if(const_cast<JSON_P::jsonPair&>(*((_type==2? _value_2 : _value_3).begin())).type()==4) {
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

    if(_verbose) std::cout << "_type: "<<_type<<": " << std::boolalpha << val_LineB << std::endl;
    switch (this->_type) {
        case 0:     out += "\""+_value_0+"\""; break;
        case 10:    out += formatNumber(_value_10, _width_val, 0); break;
        case 11:    out += formatNumber(_value_11, _width_val, _str_decimal_precision); break;
        case 12:    out += formatNumber(_value_12, _width_val, _str_decimal_precision); break;
        case 2: {
            auto itr = _value_2.begin();
            for(size_t i=0; i<_value_2.size(); i++) {
                std::string _temp = (*itr).toStr(_styleOpt, _indent, _only_value, len1_key, len1_val, false, indentSub);
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
                ++itr;
            }
            break;
        }
        case 3: {
            auto itr = _value_3.begin();
            for(size_t i=0; i<_value_3.size(); i++) {
                
                std::string _temp = (*itr).toStr(_styleOpt, _indent, _only_value, len1_key, len1_val, true, indentSub);
                
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
                ++itr;
            }
            break;
        }
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

    std::list<JSON_P::jsonPair>& toReturn = (_type==2? _value_2 : _value_3);

    int valSize = toReturn.size();
    assert(_idx<valSize);
    if(_idx<0) {
        assert(abs(_idx)<=valSize);
        _idx = static_cast<int>(valSize) + _idx;
    }
    auto itr = toReturn.begin();
    std::advance(itr, _idx);
    return *itr;
}

JSON_P::jsonPair& JSON_P::jsonPair::operator= (std::string _newVal) {
    assert(_init);

    switch (_type) {
        case 2: _value_2.clear(); break;
        case 3: _value_3.clear(); break;
    }
    _type = 0;
    _value_0 = _newVal;
    return *this;
}
JSON_P::jsonPair& JSON_P::jsonPair::operator= (int _newVal) {
    assert(_init);
    switch (_type) {
        case 2: _value_2.clear(); break;
        case 3: _value_3.clear(); break;
    }
    _type = 10;
    _value_10 = _newVal;
    return *this;
}
JSON_P::jsonPair& JSON_P::jsonPair::operator= (float _newVal) {
    assert(_init);
    switch (_type) {
        case 2: _value_2.clear(); break;
        case 3: _value_3.clear(); break;
    }
    _type = 11;
    _value_10 = _newVal;
    return *this;
}
JSON_P::jsonPair& JSON_P::jsonPair::operator= (double _newVal) {
    assert(_init);
    switch (_type) {
        case 2: _value_2.clear(); break;
        case 3: _value_3.clear(); break;
    }
    _type = 12;
    _value_12 = _newVal;
    return *this;
}
JSON_P::jsonPair& JSON_P::jsonPair::operator= (std::initializer_list<JSON_P::jsonPair> _newVal) {
    assert(_init);

    int _vecType;
    if(_newVal.size()>0) _vecType = ((*(_newVal.begin()))._onlyVal? 3 : 2);
    else _vecType = 2;

    if(_vecType!=_type) {
        switch (_type) {
            case 2: _value_2.clear(); break;
            case 3: _value_3.clear(); break;
        }
        _type = _vecType;
    }

    std::list<jsonPair>* _contR = (_vecType==2? &_value_2 : &_value_3);
    (*_contR) = _newVal;

    for(jsonPair& _pair : *_contR) _pair.setParent(this);

    return *this;
}
JSON_P::jsonPair& JSON_P::jsonPair::operator= (std::vector<JSON_P::jsonPair> _newVal) {
    assert(_init);

    int _vecType;
    if(_newVal.size()>0) _vecType = (_newVal[0]._onlyVal? 3 : 2);
    else _vecType = 2;

    if(_vecType!=_type) {
        switch (_type) {
            case 2: _value_2.clear(); break;
            case 3: _value_3.clear(); break;
        }
        _type = _vecType;
    }

    std::list<jsonPair>* _contR = (_vecType==2? &_value_2 : &_value_3);
    (*_contR) = std::list<jsonPair>(_newVal.begin(), _newVal.end());

    for(jsonPair& _pair : *_contR) _pair.setParent(this);

    return *this;
}
JSON_P::jsonPair& JSON_P::jsonPair::operator= (std::list<JSON_P::jsonPair> _newVal) {
    assert(_init);

    int _vecType;
    if(_newVal.size()>0) _vecType = (_newVal.front()._onlyVal? 3 : 2);
    else _vecType = 2;

    if(_vecType!=_type) {
        switch (_type) {
            case 2: _value_2.clear(); break;
            case 3: _value_3.clear(); break;
        }
        _type = _vecType;
    }

    std::list<jsonPair>* _contR = (_vecType==2? &_value_2 : &_value_3);
    (*_contR) = _newVal;

    for(jsonPair& _pair : *_contR) _pair.setParent(this);

    return *this;
}
JSON_P::jsonPair& JSON_P::jsonPair::operator= (bool _newVal) {
    assert(_init);
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
std::list<JSON_P::jsonPair>& JSON_P::jsonPair::get2() {
    assert(_type==2);
    return this->_value_2;
}
std::list<JSON_P::jsonPair>& JSON_P::jsonPair::get3() {
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

    this->loadFile(filename, _verbose);

}

void JSON_P::Parser::loadFile(
    std::string filename,
    bool _verbose
) {
    // JSON_P::jsonPair    _json;
    JSON_P::jsonPair*   _curr = &_json;
    
    std::list<JSON_P::jsonPair>   _tempPairs;
    /**
     *  {
     *      pair0,
     *      {
     *          subPair1,
     *          subPair2,
     *      }
     *  }
     * 
     * Location of the jsonPair in `_tempPairs[i]` relative to itself in two dim format:
     *    - [0]/.x - is the "absolute" "depth"/"index", as in the value of [i]
     *    - [1]/.y - if [0] value is an array or a json object (which it has to be if another jsonPair/_tempPairs-element is indexed to it), [1] refers to the element indenx
     * 
     */
    std::vector<pos2d> _pairLoc;

    std::fstream _file;
    _file.open(filename, std::fstream::in);
    if(!_file.is_open()) {
        throw std::invalid_argument("file \""+filename+"\" could not be opened");
    }
    else {}

    // bool    show_key    = true;     // whether the json "container" that where this is currently located is a json object (=true) or a json array (=false)
    bool    inStr       = false;    // whether the characters are inside a string
    // // bool    ignChar     = false;    // whether the preceding character was `'\'`
    // bool    isKey       = true;     // whether the current reading is of the `key` in key-value pair
    // /**
    //  * Type of the json pair that is currently read:
    //  *  - ` 0` - `std::string`
    //  *  - `10` - `int`
    //  *  - `11` - `float`
    //  *  - `12` - `double`
    //  *  - ` 2` - `json object`
    //  *  - ` 3` - `array`
    //  *  - ` 4` - `bool`
    //  *  - ` 5` - `null`
    //  * 
    //  * Contains all the types of the current and everything "above" jsonPair's
    //  */
    // std::vector<int> pairType;

    // std::vector<JSON_P::jsonPair*>  storedAt;
    // std::vector<int> pairDepth;     // number of "levels" the current reading is stored
    // std::vector<int> pairElement;   // index to the element of the json object/array that *this* jsonPair is stored at

    int absIdx      = -1; // depth of the current _curr

    // int currDepth   = -1;
    // int currElem    =  0;
    // int currType    = -1;

    // bool isDef_key  = false;    // isDefined: key
    // bool isDef_val  = false;    // isDefined: value

    // bool pairCreat  = false;    // whether the jsonPair hass been created

    /**
     * Whether a new jsonPair is to be created but hasn't been created yet.
     *  - if `=false` then the jsonPair has been created and `_curr` is updated to it.
     * 
     * @warning DOES NOT DEFINE WHETHER VALUE HAS BEEN DEFINED.
     * To check if the value is defined then check if `JSON_P::jsonPair::type()` returns -1 (this'd mean it's not defiend)
     */
    bool newPair    = true;

    // std::string Key = "";
    std::string strPiece = "";
    char c;

    while(_file.get(c)) {

        // if(isKey && inStr) {
        // }
        std::cerr << "\n \"" << (c=='\n'? "\\n" : std::string(1, c)) << "\" init: ";
        std::cerr << " type:"<<_curr->type();
        std::cerr << " inStr:"<<std::boolalpha << inStr;

        if(inStr) {
            if(c=='\\') {
                _file.get(c);
                switch (c) {
                    case 'n':   strPiece += '\n';   break;
                    case '\"':  strPiece += '\"';   break;
                    case '\\':  strPiece += '\\';   break;
                    default:    strPiece += c;      break;
                }
                continue;
            }
            std::cerr << " c0";
            if(c=='\"') { //closing strPiece
                std::cerr << " " << std::boolalpha << newPair;
                if(newPair) { //create first instance of jsonPair WITH keys
                    // std::cout << "-- -- -- --" << _curr->type() << std::endl;

                    std::cerr << " c0.1";

                    if(_curr->type()==2)_curr->append(JSON_P::jsonPair(strPiece));
                    else                _curr->append(JSON_P::jsonPair("_\"_", strPiece, true));
                    std::cerr << " »»="<<_curr->operator[](-1).getParent();

                    std::cerr << " c0.2";
                    _curr = &_curr->operator[](-1);
                    if(_curr->type()==2 || _curr->type()==3) {
                        std::cerr << " size:"<<_curr->size();
                    }
                    else {
                    }
                    std::cerr << " VIA STR: NEW APPEND: " << _curr->type();
                    newPair = false;
                }
                else { //a json string value has been "read"
                    *_curr = strPiece;
                    std::cout <<" "<< _curr->type() << "=TYPE "<<std::endl;
                    /**
                     * `newPair` will only be set to `true` during a clear indicator of a new jsonPair, example:
                     *  - `','`
                     */
                }
                inStr = false;
                strPiece.clear();
                
                continue;
            }
            std::cerr << " c1";

            strPiece += c;
            continue;
        }
        
        std::cerr << " c2";

        switch (c) { //outside string in json object/array
            case ' ': //we ignore space characters that aren't inside strings.
            case '\n': continue; break; //we ignore newline characters that aren't inside strings
            case '\"':
                inStr = true;
                break;
            case '{': //create json object jsonPair
                //vec creations::
                if(newPair) {
                    // storedAt.push_back(&*(--_tempPairs.end())); //iterator to jsonPair that this new one is stored at
                    // /**
                    //  * Create the new jsonPair element in _tempPairs and update all the related variables/containers
                    //  * 
                    //  */
                    // _tempPairs.push_back(JSON_P::jsonPair("", {}, false));
                    // _pairLoc.push_back(pos2d(currDepth, currElem)); //update where the "new" jsonPair is located (index to "parent" jsonPair)
                    // // current++;
                    // currDepth++;

                    if(_json.init()) {
                        _curr->append(JSON_P::jsonPair("_{_", {}, false));
                        _curr = &_curr->operator[](-1);
                    }
                    else {
                        _json = jsonPair("", {}, false);
                        std::cout << " "<<_curr->type() << "=TYPE "<<std::endl;
                        _json._onlyVal = true;
                        
                    }
                }
                else {
                    /**
                     * Update pre-existing json (_tempPairs[current])
                     * 
                     */
                    *_curr = std::vector<JSON_P::jsonPair>();
                    std::cout << " "<<_curr->type() << "=TYPE "<<std::endl;
                    newPair = true;
                }
                _curr->isArray(false);
                break;
            case '[': //create json array jsonPair
                //vec creations::

                if(newPair) {
                    if(_json.init()) {
                        _curr->append(JSON_P::jsonPair("_[_", {}, true));
                        _curr = &_curr->operator[](-1);
                        std::cout << " "<<_curr->type() << "=TYPE "<<std::endl;
                    }
                    else {
                        _json = jsonPair("", {}, true);
                        _json._onlyVal = true;
                    }
                }
                else {
                    *_curr = std::vector<JSON_P::jsonPair>();
                    newPair = true;
                    std::cout << " "<<_curr->type() << "=TYPE "<<std::endl;
                }
                _curr->isArray(true);

                // pairType.push_back(3);
                // isKey = false;
                break;
            case '}': //close json object jsonPair
                newPair = false;
                
                _curr = _curr->getParent();
                // pairType.erase(--pairType.end());
                // pairDepth.erase(--pairDepth.end());
                // pairElement.erase(--pairElement.end());

                break;
            case ']': //close json array jsonPair
                newPair = false;

                _curr = _curr->getParent();
                // pairType.erase(--pairType.end());
                // pairDepth.erase(--pairDepth.end());
                // pairElement.erase(--pairElement.end());
                break;
            case ',': //new element in either json-array or json-object
                std::cerr << " c','0";
                if(_curr->type()==-1) { //a value hasn't been defined/set/given
                    //main method of defining non-string, non-json-array/object to jsonPair
                    if(strPiece=="false")       *_curr = false;
                    else if(strPiece=="true")   *_curr = true;
                    else {
                        static std::string numberesque = ".-+012345679";
                        std::string numberStr = "";
                        int numType = 10;
                        int numDecimals = 0;
                        for(char cc : strPiece) {
                            for(char ccc : numberesque) {
                                if(cc == ccc) {
                                    numberStr += cc;
                                    if(numType!=10) numDecimals++;
                                    if(cc=='.') numType = 11;
                                    break;
                                }
                            }

                        }
                        if(numDecimals>7) numType = 12;
                        std::cout << " \t\t[PARSED NUMBER:"<<numberStr<<"]";
                        switch (numType) {
                        case 10: *_curr = std::stoi(numberStr); break;
                        case 11: *_curr = std::stof(numberStr); break;
                        case 12: *_curr = std::stod(numberStr); break;
                        default:
                            break;
                        }
                    }
                    std::cout << " "<<_curr->type() << "=TYPE "<<std::endl;
                    strPiece.clear();
                }
                // if(_curr->operator[](-1).type() == 2 || _curr->operator[](-1).type() == 3) {}
                // else
                std::cerr << " *»" << _curr;
                _curr = _curr->getParent();
                std::cerr << " *»" << _curr;
                std::cerr << " c','1";
                newPair = true;
                // pairElement.back()++;
                // isKey = (pairType.back()==3? false : true);
                break;
            case ':': //define jsonPair value (though must not rely on this to detect whether something is to be defined as a value)
                // isKey = false;
                break;
            default:
                strPiece += c;
                break;
        }

    }
    // std::cout << _temp.type() << std::endl;
    // this->_json = _temp;

}

