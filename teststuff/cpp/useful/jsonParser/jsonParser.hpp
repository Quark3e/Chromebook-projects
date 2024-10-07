
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


    /**
     * A class to represent the "key-value" pair found in JSON syntax/string.
     * The class can be used as a representation for the values, for the use in json arrays.
     * 
     */
    class jsonPair {
            std::string _info_name = "JSON_P::jsonPair";


            std::string                 _value_0;
            int                         _value_10;
            float                       _value_11;
            double                      _value_12;
            std::list<jsonPair>         _value_2;   // object(/s) `{}`
            std::list<jsonPair>         _value_3;   // array `[]`
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
            int _type   = -1;
            bool _init  = false;

            jsonPair* _parent = nullptr;
            int _siblingIndex = 0;

        public:
            bool init() const;

            /**
             * @brief Whether the instance of this class will be passed as purely value
             * @note does not necessarily mean the stored type is a json array. The only way
             * to define if value is a json array or object is via `_type` or type()` for public
             * access
             */
            bool _onlyVal = false;
            
            std::string key;

            /**
             * @brief Construct a new jsonPair object.
             * This is the default constructor.
             */
            jsonPair() = default;
            /**
             * @brief Construct a new jsonPair object
             * 
             * @param _key `std::string` of the json pair key
             * @param parent address to the parent jsonPair that this jsonPair is stored in.
             */
            jsonPair(std::string _key, jsonPair* parent=nullptr);
            /**
             * @brief Construct a new jsonPair object.
             * 
             * @param _key `std::string` of the json pair key
             * @param _value `std::string` value to store
             * @param onlyVal whether to only display/use the value of the key-value pair
             * @param parent address to the parent jsonPair that this jsonPair is stored in.
             */
            jsonPair(std::string _key, std::string _value, bool onlyVal=false, jsonPair* parent=nullptr);
            /**
             * @brief Construct a new jsonPair object.
             * 
             * @param _key `std::string` of the json pair key
             * @param _value `int` value to store
             * @param onlyVal whether to only display/use the value of the key-value pair
             * @param parent address to the parent jsonPair that this jsonPair is stored in.
             */
            jsonPair(std::string _key, int _value, bool onlyVal=false, jsonPair* parent=nullptr);
            /**
             * @brief Construct a new jsonPair object.
             * 
             * @param _key `std::string` of the json pair key
             * @param _value `float` value to store
             * @param onlyVal whether to only display/use the value of the key-value pair
             * @param parent address to the parent jsonPair that this jsonPair is stored in.
             */
            jsonPair(std::string _key, float _value, bool onlyVal=false, jsonPair* parent=nullptr);
            /**
             * @brief Construct a new jsonPair object.
             * 
             * @param _key `std::string` of the json pair key
             * @param _value `double` value to store
             * @param onlyVal whether to only display/use the value of the key-value pair
             * @param parent address to the parent jsonPair that this jsonPair is stored in.
             */
            jsonPair(std::string _key, double _value, bool onlyVal=false, jsonPair* parent=nullptr);
            /**
             * @brief Construct a new jsonPair object.
             * 
             * @param _key `std::string` of the json pair key
             * @param _value `std::initializer_list<JSON_P::jsonPair>` json object/array to store
             * @param isArray whether `_value` is a json array (=true) or a json object (=false)
             * @param parent address to the parent jsonPair that this jsonPair is stored in
             */
            jsonPair(std::string _key, std::initializer_list<jsonPair> _value, bool isArray, jsonPair* parent=nullptr);
            /**
             * @brief Construct a new jsonPair object.
             * 
             * @param _key `std::string` of the json pair key
             * @param _value `std::vector<JSON_P::jsonPair>` json object/array to store
             * @param isArray whether `_value` is a json array (=true) or a json object (=false)
             * @param parent address to the parent jsonPair that this jsonPair is stored in
             */
            jsonPair(std::string _key, std::vector<jsonPair> _value, bool isArray, jsonPair* parent=nullptr);
            /**
             * @brief Construct a new jsonPair object
             * 
             * @param _key `std::string` of the json pair key
             * @param _value `std::list<JSON_P::jsonPair>` json object/array to store
             * @param isArray whether `_value` is a jsonPair that this jsonPair is stored in
             * @param parent address to the parent jsonpair that this jsonPair is stored in
             */
            jsonPair(std::string _key, std::list<jsonPair> _value, bool isArray, jsonPair* parent=nullptr);
            /**
             * @brief Construct a new jsonPair object.
             * 
             * @param _key `std::string` of the json pair key
             * @param _value `bool` value to store
             * @param onlyVal whether to only display/use the value of the key-value pair
             * @param parent address to the parent jsonPair that this jsonPair is stored in.
             */
            jsonPair(std::string _key, bool _value, bool onlyVal=false, jsonPair* parent=nullptr);

            void append(jsonPair newPair);
            void insert(int idx, jsonPair newPair);
            void erase(int idx);

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

            const size_t size();

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
            jsonPair& operator= (std::initializer_list<jsonPair> _newVal);
            jsonPair& operator= (std::vector<jsonPair> _newVal);
            jsonPair& operator= (std::list<jsonPair> _newVal);
            jsonPair& operator= (bool _newVal);

            std::string&            get0();
            int&                    get10();
            float&                  get11();
            double&                 get12();
            std::list<jsonPair>&    get2();
            std::list<jsonPair>&    get3();
            bool&                   get4();

            jsonPair* getParent();
            int getSiblingIdx();

            void setParent(jsonPair* parent);
            void setSiblingIndex(int idx);
            
            /**
             * @brief Boolean access function to set whether the current stored container is a
             * json object (`isArray`=false, _type=2) or a json array (`isArray`=true, _type=3)
             * 
             * @param isArray true (json array) or false (json object)
             */
            void isArray(bool isArray);
    };
    

    class Parser {
        private:

        public:
            jsonPair _json;


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