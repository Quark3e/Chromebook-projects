
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


namespace JSON_P {

    struct jsonPair;


    /**
     * A class to represent the "key-value" pair found in JSON syntax/string.
     * The class can be used as a representation for the values, for the use in json arrays.
     * 
     */
    class jsonPair {

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

            jsonPair(std::string _key="", std::string _value);
            jsonPair(std::string _key="", int _value);
            jsonPair(std::string _key="", float _value);
            jsonPair(std::string _key="", double _value);
            // jsonPair(std::string _key="", jsonPair* _value);
            jsonPair(std::string _key="", std::initializer_list<jsonPair> _value, bool isArray);
            jsonPair(std::string _key="", std::vector<jsonPair> _value, bool isArray);
            jsonPair(std::string _key="", bool _value);

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

            std::string const& toStr() const;
            friend std::ostream& operator<<(std::ostream &os, const JSON_P::jsonPair& jP);


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