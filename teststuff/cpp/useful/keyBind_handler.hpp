#pragma once
#ifndef HPP__KEYBIND_HANDLER
#define HPP__KEYBIND_HANDLER

#include <string>
#include <vector>
#include <initializer_list>

#include <chrono>


/// @brief DIY keyBind handler namespace
namespace DIY_KBH {
    using _funcDef = void(*)(void);
    
    /// @brief Number of instances that a key can not be called for it to still hold the "called" state.
    inline size_t setting_callDecay = 2;


    /// @brief Instance struct used for holding a single keybind configuration
    struct keyBind_handler_unit {
        std::string         _label;
        std::vector<int>    _keys;
        _funcDef            _callFunc       = nullptr;
        bool                _orderImportant = false;
        bool                _isolImportant  = true;
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
         */
        std::vector<keyBind_handler_unit>   _keyBinds;
        // Positive integer value to assosciate with a keybind call to hold the decay value. 
        std::vector<int>                    _callDecay;
        // Boolean for whether the keyBind decay value has reached the "opposite decay value" for it's callEdge after latest funcCall
        std::vector<bool>                   _released;
        // Boolean for whether the keyBind has been called in this frame.
        std::vector<bool>                   _called;

        public:
        void update(std::vector<int> _pressed_keys);
 
        
        keyBind_handler();
        keyBind_handler(keyBind_handler_unit _kbh_inst);
        keyBind_handler(std::string _label, std::vector<int> _keys, void (*_func)(void), bool _ordImportant=false, bool _isolImportant=false, bool _callSignalEdge=true);
        keyBind_handler(std::initializer_list<keyBind_handler_unit> _kbhi_cont);
        keyBind_handler(std::vector<keyBind_handler_unit> _kbhi_cont);

        ~keyBind_handler();

        size_t size() const;

        // int add(keyBind_handler_unit _newKBH);
        // int add(std::string _label, std::vector<int> _keys, _funcDef _callFunc, bool _ordImportant=false, bool _isolImportant=false, bool _callEdge=true);

        int edit(std::string _label, std::vector<int> _newKeys, bool _call_except=true);

        keyBind_handler_unit get(std::string _label);
        keyBind_handler_unit get(std::string _label) const;

        /**
         * @brief Bool for whether the _label assosciated keybind is clicked in current program loop/iteration according to `callEdge` parameter (`callEdge==true`->initial press signal; `callEdge==false`->press release signal)
         * @note if the keyBind is held, this function will only return true during the initial signal (according to callEdge as previously mentioned)
         * @param _label `std::string` of keybind label
         * @return true if keybind is pressed(callEdge*) in current update call.
         * @return false every other scenario.
         */
        bool clicked(std::string _label);
        /**
         * @brief Bool for whether the _label assosciated keybind is currently held
         * 
         * @param _label `std::string` of keybind label
         * @return true if keybind is pressed(callEdge*)
         * @return false every other scenario
         */
        bool pressing(std::string _label);
    };

}


#endif