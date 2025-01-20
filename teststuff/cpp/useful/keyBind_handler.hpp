#pragma once
#ifndef HPP__KEYBIND_HANDLER
#define HPP__KEYBIND_HANDLER

#include <string>
#include <vector>
#include <initializer_list>

/// @brief DIY keyBind handler namespace
namespace DIY_KBH {
    using _funcDef = void(*)(void);
    
    /// @brief Number of instances that a key can not be called for it to still hold the "called" state.
    size_t setting_callDecay = 0;


    /// @brief Instance struct used for holding a single keybind configuration
    struct keyBind_handler_unit {
        std::string         _label;
        std::vector<int>    _keys;
        _funcDef            _callFunc;
        bool                _orderImportant = false;
        bool                _isolImportant  = false;
        bool                _callEdge       = true;
    };


    /**
     * Handler for detecting key binds/combinations and calling assosciated functions:
     *  - key   type: `int`
     *  - func  type: `void (*)(void)`
     */
    class keyBind_handler {
        private:
        // std::vector<std::string>        _label;         // label for the keybind.
        // std::vector<std::vector<int>>   _keybinds;      // Main container to hold the keybind combinations.
        // std::vector<_funcDef>           _callFuncs;     // Function to call when keybind is called.
        // std::vector<bool>               _orderImportant;// Boolean for whether order of the keybind combination is important. Default is false
        // std::vector<bool>               _callEdge;      // What edge of the signal to call the assosciated function in: `true`-rising edge; `false`-falling edge. Default to `true`

        /**
         * Main container that holds the configuration and values of every keybind
         * 
         */
        std::vector<keyBind_handler_unit>   _keyBinds;
        // Positive integer value to assosciate with a keybind call to hold the decay value. 
        std::vector<size_t>                 _callDecay;
        // Boolean for whether the keyBind decay value has reached the "opposite decay value" for it's callEdge after latest funcCall
        std::vector<bool>                   _released;

        public:
        void update(std::vector<int> _pressed_keys);
 
        
        keyBind_handler();
        keyBind_handler(keyBind_handler_unit _kbh_inst);
        keyBind_handler(std::string _label, std::vector<int> _keys, void (*_func)(void), bool _ordImportant=false, bool _isolImportant=false, bool _callSignalEdge=true);
        keyBind_handler(std::initializer_list<keyBind_handler_unit> _kbhi_cont);
        keyBind_handler(std::vector<keyBind_handler_unit> _kbhi_cont);

        ~keyBind_handler();

        size_t size() const;

        int add(keyBind_handler_unit _newKBH);
        int add(std::string _label, std::vector<int> _keys, _funcDef _callFunc, bool _ordImportant=false, bool _isolImportant=false, bool _callEdge=true);

        // int edit(std::string _label);
        


    };

}


#endif