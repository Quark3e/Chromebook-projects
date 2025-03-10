
#include <scienceUnit.hpp>


int SCIU::solveE(double _value, double *_mantissa) {
    int E = (_value==0? 0 : int(std::roundf(std::log10(std::abs(_value)))));

    if(_mantissa) *_mantissa = _value * pow(10, -(E));

    return E;
}
int SCIU::solveE_specif(double _value, double *_mantissa, int n_range_min, int n_range_max) {
    static int checkSize[3] = {50, 10, 1};
    size_t cS = 0;

    int nLim[2] = {E_lim__min, E_lim__max};
    
    if(nLim[1]-nLim[0]>100) cS = 0;
    else if(nLim[1]-nLim[0]>50) cS = 1;
    else cS = 2;

    for(size_t i=cS; i<3; i++) {
        for(int n=nLim[0]; n<nLim[1]; n+=checkSize[i]) {
            double powVar = pow(10, n);
            if(_value >= powVar) {
                nLim[0] = 0;
            }
            else if(_value < powVar) {
                nLim[1] = n;
                break;
            }
        }
    }

    if(_mantissa) *_mantissa = _value * pow(10, nLim[1]-1);

    return nLim[1]-1;
}


double SCIU::scienceUnit::value(prefix_metric _prefix=prefix_metric::norm) {
    double val = 0;
    if(__E > E_lim__max) throw std::runtime_error("ERROR: number larger than double max lim.");
    if(__E < E_lim__min) throw std::runtime_error("ERROR: number smaller than double min lim.");

    return (__value * pow(10, __E));
}

SCIU::scienceUnit::scienceUnit(/* args */) {

}
SCIU::scienceUnit::scienceUnit(double _value, prefix_metric _prefix=prefix_metric::norm, bool _parseNumber)
: __E(0), __value(_value), __prefix(_prefix) {
    /**
     * Extract E number from _value and update member variables.
     * 
     */

    if(_parseNumber) {
        double mantissa = 0;
        
        this->__E = solveE(_value, &mantissa);
        this->__value   = mantissa;
        this->__prefix  = _prefix;
    }

}
SCIU::scienceUnit::scienceUnit(double _value, int _E, prefix_metric _prefix=prefix_metric::norm, bool _parseNumber)
: __E(_E), __value(_value), __prefix(_prefix) {
    /**
     * Extract E number from _value, modify it if _parseNumber=true, otherwise define it directly, so the stored E is `_E` and then update member variables.
     * 
     */

    if(_parseNumber) {
        double mantissa = 0;
        int temp_E = solveE(_value, &mantissa);
    
        this->__E = _E;
        this->__value   = mantissa*pow(10, -(_E-temp_E));
        this->__prefix  = _prefix;
    }

}
SCIU::scienceUnit::scienceUnit(const scienceUnit& _copy)
: __E(_copy.__E), __value(_copy.__value), __prefix(_copy.__prefix) {

}
SCIU::scienceUnit::~scienceUnit() {

}

SCIU::scienceUnit& SCIU::scienceUnit::operator=(const scienceUnit& _copy) {

    this->__E       = _copy.__E;
    this->__value   = _copy.__value;
    this->__prefix  = _copy.__prefix;

}

SCIU::scienceUnit& SCIU::scienceUnit::operator+(const scienceUnit& _copy) {
    /**
     * Will convert _copy's values to this object instance's _prefix exponent values.
     * 
     */

    int delta_E = int(_copy.__prefix) - int(__prefix);

    return scienceUnit(
        __value + _copy.__value * pow(10, (_copy.__E+delta_E)-__E),
        __E,
        __prefix,
        false
    );
}
SCIU::scienceUnit& SCIU::scienceUnit::operator-(const scienceUnit& _copy) {

    int delta_E = int(_copy.__prefix) - int(__prefix);

    return scienceUnit(
        __value - _copy.__value * pow(10, (_copy.__E+delta_E)-__E),
        __E,
        __prefix,
        false
    );
}
SCIU::scienceUnit& SCIU::scienceUnit::operator*(const scienceUnit& _copy) {

    int delta_E = int(_copy.__prefix) - int(__prefix);

    return scienceUnit(
        __value * _copy.__value,
        __E + (_copy.__E + delta_E),
        __prefix,
        false
    );
}
SCIU::scienceUnit& SCIU::scienceUnit::operator/(const scienceUnit& _copy) {
    
    int delta_E = int(_copy.__prefix) - int(__prefix);

    return scienceUnit(
        __value * _copy.__value,
        __E - (_copy.__E + delta_E),
        __prefix,
        false
    );
}

SCIU::scienceUnit& SCIU::scienceUnit::operator+=(const scienceUnit& _copy) {

    int delta_E = int(_copy.__prefix) - int(__prefix);

    this->__value += _copy.__value * pow(10, (_copy.__E+delta_E)-__E);

    return *this;
}
SCIU::scienceUnit& SCIU::scienceUnit::operator-=(const scienceUnit& _copy) {

    int delta_E = int(_copy.__prefix) - int(__prefix);

    this->__value -= _copy.__value * pow(10, (_copy.__E+delta_E)-__E);

    return *this;
}
SCIU::scienceUnit& SCIU::scienceUnit::operator*=(const scienceUnit& _copy) {

    int delta_E = int(_copy.__prefix) - int(__prefix);

    this->__value   *= _copy.__value;
    this->__E       += (_copy.__E + delta_E);

    return *this;
}
SCIU::scienceUnit& SCIU::scienceUnit::operator/=(const scienceUnit& _copy) {

    int delta_E = int(_copy.__prefix) - int(__prefix);

    this->__value   /= _copy.__value;
    this->__E       -= (_copy.__E + delta_E);

    return *this;
}

/**
 * @brief Change the set prefix type and update the stored numbers correctly.
 * @param _newPrefix `prefix_metric` enum item to update to.
 */
void SCIU::scienceUnit::update_prefix(prefix_metric _newPrefix) {

    int delta_E = int(_newPrefix) - int(__prefix);

    this->__value   *= pow(10, -delta_E);
    this->__E       += int(_newPrefix);

}
/**
 * @brief Edit the prefix to a new prefix enum item without updating the values to correspond to the change,
 * i.e. changing a value from `deci` to `giga` won't increase the E by appropriate value.
 * @param _newPrefix `prefix_metric` enum item to edit to.
 */
void SCIU::scienceUnit::edit_prefix(prefix_metric _newPrefix) {

    this->__prefix = _newPrefix;
}


bool SCIU::scienceUnit::operator==(const scienceUnit& _obj) {
    int delta_E = int(_obj.__prefix) - int(__prefix);

    double manissa[2] = {__value, _obj.__value};
    int exp[2] = {__E, _obj.__E};

    exp[0] += solveE_specif(manissa[0], &manissa[0], -3, 3);
    exp[1] += solveE_specif(manissa[1], &manissa[1], -3, 3);

    return (
        roundf(manissa[0]*pow(10, pref__fractionals))*pow(10, -pref__fractionals) == roundf(manissa[1]*pow(10, pref__fractionals))*pow(10, -pref__fractionals) &&
        (__E+exp[0]==_obj.__E+exp[1])
    );
}
bool SCIU::scienceUnit::operator!=(const scienceUnit& _obj) {
    int delta_E = int(_obj.__prefix) - int(__prefix);

    double manissa[2] = {__value, _obj.__value};
    int exp[2] = {__E, _obj.__E};

    exp[0] += solveE_specif(manissa[0], &manissa[0], -3, 3);
    exp[1] += solveE_specif(manissa[1], &manissa[1], -3, 3);

    return !(
        roundf(manissa[0]*pow(10, pref__fractionals))*pow(10, -pref__fractionals) == roundf(manissa[1]*pow(10, pref__fractionals))*pow(10, -pref__fractionals) &&
        (__E+exp[0]==_obj.__E+exp[1])
    );
}
bool SCIU::scienceUnit::operator<=(const scienceUnit& _obj) {
    int delta_E = int(_obj.__prefix) - int(__prefix);

    return (
        (__E >= _obj.__E && __value >= _obj.__value) || 
        (__value <= _obj.__value && __E > _obj.__E)
    );
}
bool SCIU::scienceUnit::operator>=(const scienceUnit& _obj) {

}
bool SCIU::scienceUnit::operator>(const scienceUnit& _obj) {

}
bool SCIU::scienceUnit::operator<(const scienceUnit& _obj) {

}
