
#pragma once
#ifndef HPP__JSON_PARSER
#define HPP__JSON_PARSER

#include <vector>
#include <initializer_list>

#include <string>
#include <math.h>

#include <fstream>
#include <memory>
#include <cassert>

#include <HC_useful/useful.hpp>


#define _WIDTH_AUTO     -69420      // Width of the value it holds
#define _WIDTH_MAX      -69421      // Number of characters for the width: Of the maximum value/element this is applied to
#define _WIDTH_MAX_KEY  -69422      // Max len of the keys in container


namespace JSON_P {

    struct jsonPair;


    /**
     * A class to represent the "key-value" pair found in JSON syntax/string.
     * The class can be used as a representation for the values, for the use in json arrays.
     * 
     */
    class jsonPair {
            std::string _info_name = "JSON_P::jsonPair";

            /**
             * @brief Whether the instance of this class will be passed as purely value
             */
            bool _onlyVal = false;

            std::string                 _value_0;
            int                         _value_10;
            float                       _value_11;
            double                      _value_12;
            std::vector<jsonPair>       _value_2;   // object(/s) `{}`
            std::vector<jsonPair>       _value_3;   // array `[]`
            bool                        _value_4;

            /**
             * type of the value
             * - `
             * - `0` - `string`
             * - `1` - `number`
             *  -- `10` - number: `int`
             *  -- `11` - number: `float`
             *  -- `12` - number: `double`
             * - `2` - `json object`:    {}
             * - `3` - `array`:     []
             * - `4` - `boolean`    true/false
             * - `5` - `null`
             */
            int _type = -1;

        public:
            std::string key;


            jsonPair(std::string _key, std::string _value);
            jsonPair(std::string _key, int _value);
            jsonPair(std::string _key, float _value);
            jsonPair(std::string _key, double _value);
            jsonPair(std::string _key, std::initializer_list<jsonPair> _value, bool isArray);
            jsonPair(std::string _key, std::vector<jsonPair> _value, bool isArray);
            jsonPair(std::string _key, bool _value);

            /**
             * type of the value
             * - `
             * - `0` - `string`
             * - `1` - `number`
             *  -- `10` - number: `int`
             *  -- `11` - number: `float`
             *  -- `12` - number: `double`
             * - `2` - `json object`:    {}
             * - `3` - `array`:     []
             * - `4` - `boolean`    true/false
             * - `5` - `null`
             */
            const int type();

            int _str_decimal_precision = 3;

            /**
             * @brief Convert the jsonPair contents into a single `std::string`
             * 
             * @param _styleOpt different options for how to style the contents (0).
             * @param _indent number of space char's `' '` to use as indent from left side
             * @param _only_value whether to only display the values
             * @param _width_key width of the key. Will extend with space char `' '` if key doesn't fill that width.
             * @param _width_val width of the value. Will extend with space char `' '` if value doesn't fill that width.
             * @param _no_key whether to not display the key(s).
             * @param _indent_pair whether to add indent from left-side on that pair (mainly the key part)
             * @return std::string const& 
             * @note (0):
             * @note  - `0` - no line-breaks/newline. All in the same line.
             * @note  - `1` - line-breaks/newline on every "level" except the "last"/"deepest" array/json-object
             * @note  - `2` - line-breaks/newline on every "level".
             * @note  - `3` - same as `1` but add newline if "last"/"deepest" "level" is a json object literal regardless if it's the "deepest" level
             */
            std::string const& toStr(
                int     _styleOpt   = 3,
                int     _indent     = 4,
                bool    _only_value = false,
                int     _width_key  = _WIDTH_AUTO,
                int     _width_val  = _WIDTH_MAX_KEY,
                bool    _no_key     = false,
                bool    _indent_pair= true
            ) const;
            mutable std::string _toStr = "";

            friend std::ostream& operator<<(std::ostream &os, const JSON_P::jsonPair& jP);

            jsonPair& operator[] (std::string _key);
            jsonPair& operator[] (int _idx);
            
            jsonPair& operator= (std::string _newVal);
            jsonPair& operator= (int _newVal);
            jsonPair& operator= (float _newVal);
            jsonPair& operator= (double _newVal);
            jsonPair& operator= (std::vector<jsonPair> _newVal);
            jsonPair& operator= (bool _newVal);

            std::string&            get0();
            int&                    get10();
            float&                  get11();
            double&                 get12();
            std::vector<jsonPair>&  get2();
            std::vector<jsonPair>&  get3();
            bool&                   get4();

    };
    

    class Parser {
        private:


        public:
            Parser() = default;
            Parser(std::string filename, bool _verbose=false);


            /**
             * @brief Load and parse a json file
             * 
             * @param filename name of the file
             * @param verbose whether to print the loaded info
             */
            void loadFile(std::string filename, bool _verbose=false);
    };


    /**
     *  {
     *      "projects": [
     *          {
     *              "date": "",
     *              "name": "",
     *              "pos" : [0, 0],
     *              "nodes": [
     *                  {
     *                      
     *                  }
     *              ],
     *              "links": [
     *                  {
     *                      
     *                  }
     *              ]
     *          }
     *      ]
     *  }
     * 
     * 
     * 
     *  {
     *      "key" : "value",
     *      "key" : [value, value, value],
     *      "key" : `value/object`{ "key":"value", "key":"value" }
     *  }
     * 
     */

};


#endif