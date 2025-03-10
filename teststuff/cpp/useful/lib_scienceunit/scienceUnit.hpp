#pragma once
#ifndef HPP_LIB_SCIENCEUNIT
#define HPP_LIB_SCIENCEUNIT


#include <cmath>
#include <limits>

#include <string>

#include <diy_dictionary.hpp>


namespace SCIU {
    enum class prefix_metric {
        pico    = -12,
        nano    = -9,
        micro   = -6,
        milli   = -3,
        centi   = -2,
        deci    = -1,
        norm    = 0, // Non prefixed, default value.
        deca    = 1,
        hecto   = 2,
        kilo    = 3,
        mega    = 6,
        giga    = 9,
        tera    = 12
    };

    DIY::typed_dict<std::string, prefix_metric> prefix_metric_string({
        {"pico" ,   prefix_metric::pico},
        {"nano" ,   prefix_metric::nano},
        {"micro",   prefix_metric::micro},
        {"milli",   prefix_metric::milli},
        {"centi",   prefix_metric::centi},
        {"deci" ,   prefix_metric::deci},
        {"norm" ,   prefix_metric::norm},
        {"deca" ,   prefix_metric::deca},
        {"hecto",   prefix_metric::hecto},
        {"kilo" ,   prefix_metric::kilo},
        {"mega" ,   prefix_metric::mega},
        {"giga" ,   prefix_metric::giga},
        {"tera" ,   prefix_metric::tera}
    });

    DIY::typed_dict<std::string, prefix_metric> prefix_metric_symbol({
        {"p" ,  prefix_metric::pico},
        {"n" ,  prefix_metric::nano},
        {"µ",   prefix_metric::micro},
        {"m",   prefix_metric::milli},
        {"c",   prefix_metric::centi},
        {"d" ,  prefix_metric::deci},
        {"" ,   prefix_metric::norm},
        {"da",  prefix_metric::deca},
        {"h",   prefix_metric::hecto},
        {"k" ,  prefix_metric::kilo},
        {"M" ,  prefix_metric::mega},
        {"G" ,  prefix_metric::giga},
        {"T" ,  prefix_metric::tera}
    });

    const int E_lim__min = -308;
    const int E_lim__max = 308;


    /**
     * @brief Get the E (exponent of 10) value from number
     * 
     * @param _value the number to get the base from
     * @param _mantissa the coefficient that's multiplied by 10^E
     * @return int the E number.
     */
    int solveE(double _value, double *_mantissa=nullptr);

    int solveE_specif(double _value, double *_mantissa=nullptr, int n_range_min=E_lim__min, int n_range_max=E_lim__max);

    class scienceUnit {
        private:
        
        /// @brief "Exponent of 10" value.
        int __E = 0;

        double __value = 0;

        prefix_metric __prefix = prefix_metric::norm;
    
        public:
        /**
         * @brief number of decimal/precision/fractional numbers to display
         * @warning This member variable is not allowed to have a value of `0` if `pref__integers==0`.
         */
        size_t pref__fractionals = 3;
        /**
         * @brief Number of whole numbers to hold before changing `E` value. Is not just for display purposes.
         * @warning This member variable is not allowed to have a value of `0` if `pref__fractionals==0`.
         * @note ex: 
         * 
         *  - `0` -> 0.11
         * 
         *  - `1` -> 9.11
         * 
         *  - `2` -> 99.11
         * 
         */
        size_t pref__integers = 0;



        /**
         * @brief Get the `double` type of the stored value for given prefix and `E`.
         * 
         * @return `double` type of the value in `{prefix}` enum item prefix.
         * @note if the converted value is outside the size of `double` then it'll throw exception:
         * 
         * - number is bigger than double -> `std::runtime_error("ERROR: number larger than double max lim.")`
         * 
         * - number if smaller than double -> `std::runtime_error("ERROR: number smaller than double min lim.")`
         */
        double value(prefix_metric _prefix=prefix_metric::norm);

        scienceUnit(/* args */);
        scienceUnit(double _value, prefix_metric _prefix=prefix_metric::norm, bool _parseNumber=true);
        scienceUnit(double _value, int _E, prefix_metric _prefix=prefix_metric::norm, bool _parseNumber=true);
        scienceUnit(const scienceUnit& _copy);
        ~scienceUnit();
    
        scienceUnit& operator=(const scienceUnit& _copy);

        scienceUnit& operator+(const scienceUnit& _copy);
        scienceUnit& operator-(const scienceUnit& _copy);
        scienceUnit& operator*(const scienceUnit& _copy);
        scienceUnit& operator/(const scienceUnit& _copy);

        scienceUnit& operator+=(const scienceUnit& _copy);
        scienceUnit& operator-=(const scienceUnit& _copy);
        scienceUnit& operator*=(const scienceUnit& _copy);
        scienceUnit& operator/=(const scienceUnit& _copy);

        /**
         * @brief Change the set prefix type and update the stored numbers correctly.
         * @param _newPrefix `prefix_metric` enum item to update to.
         */
        void update_prefix(prefix_metric _newPrefix);
        /**
         * @brief Edit the prefix to a new prefix enum item without updating the values to correspond to the change,
         * i.e. changing a value from `deci` to `giga` won't increase the E by appropriate value.
         * @param _newPrefix `prefix_metric` enum item to edit to.
         */
        void edit_prefix(prefix_metric _newPrefix);


        bool operator==(const scienceUnit& _obj);
        bool operator!=(const scienceUnit& _obj);
        bool operator<=(const scienceUnit& _obj);
        bool operator>=(const scienceUnit& _obj);
        bool operator>(const scienceUnit& _obj);
        bool operator<(const scienceUnit& _obj);


        

    };

} // namespace SCIU




#endif // HPP_LIB_SCIENCEUNIT