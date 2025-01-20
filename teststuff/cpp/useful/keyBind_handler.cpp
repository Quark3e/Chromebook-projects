#include "keyBind_handler.hpp"

#include <useful.hpp>

void DIY_KBH::keyBind_handler::update(std::vector<int> _pressed_keys) {

    for(size_t i=0; i<_keyBinds.size(); i++) {
        bool _match = false;
        bool _call  = false;
        size_t newDecayVal = _callDecay[i];
        
        if(
            ( // valid instances
                !_keyBinds[i]._isolImportant ||
                (_keyBinds[i]._isolImportant && _keyBinds[i]._keys.size()==_pressed_keys.size())
            ) && ( // general valid check
                _keyBinds[i]._keys.size()>=_pressed_keys.size()
            )
        ) {
            if(find_vector<int>(_keyBinds[i]._keys, _pressed_keys, _keyBinds[i]._orderImportant) != -1) {
                /// Keybind is pressed
                
                _match = true;
                newDecayVal = _callDecay[i]+1;
            }
        }
        else {
            /// Keybind is not pressed

            newDecayVal = _callDecay[i]-1;
        }


        if(_released[i]) {
            if(newDecayVal==0 && _callDecay[i]>0 && !_keyBinds[i]._callEdge && _released[i]) {
                /// Falling edge signal func call
                _call = true;
            }
            else if(newDecayVal==setting_callDecay && _callDecay[i]<setting_callDecay && _keyBinds[i]._callEdge && _released[i]) {
                // Rising edge signal func call
                _call = true;
            }
        }

        if(_call) {
            _keyBinds[i]._callFunc();
            _released[i] = false;
        }
        
        /// Update _released
        if((_keyBinds[i]._callEdge && newDecayVal==0) || !_keyBinds[i]._callEdge && newDecayVal==setting_callDecay) _released[i] = true;

        if(newDecayVal<0)                   newDecayVal = 0;
        if(newDecayVal>setting_callDecay)   newDecayVal = setting_callDecay;
        
        _callDecay[i] = newDecayVal;
    }


}

        
DIY_KBH::keyBind_handler::keyBind_handler() {}
DIY_KBH::keyBind_handler::keyBind_handler(keyBind_handler_unit _kbh_inst) {
    _keyBinds.push_back(_kbh_inst);
    _callDecay.push_back(0);
    _released.push_back(_kbh_inst._callEdge);
}
DIY_KBH::keyBind_handler::keyBind_handler(std::string _label, std::vector<int> _keys, void (*_func)(void), bool _ordImportant, bool _isolImportant, bool _callSignalEdge) {
    _keyBinds.push_back(keyBind_handler_unit{_label, _keys, _func, _ordImportant, _isolImportant, _callSignalEdge});
    _callDecay.push_back(0);
    _released.push_back(_callSignalEdge);
}
DIY_KBH::keyBind_handler::keyBind_handler(std::initializer_list<keyBind_handler_unit> _kbhi_cont): _keyBinds{_kbhi_cont} {
    _callDecay = std::vector<size_t>(_kbhi_cont.size(), 0);
    for(size_t i=0; i<_keyBinds.size(); i++) {
        _released.push_back(_keyBinds[i]._callEdge);
    }
}
DIY_KBH::keyBind_handler::keyBind_handler(std::vector<keyBind_handler_unit> _kbhi_cont): _keyBinds{_kbhi_cont} {
    _callDecay = std::vector<size_t>(_kbhi_cont.size(), 0);
    for(size_t i=0; i<_keyBinds.size(); i++) {
        _released.push_back(_keyBinds[i]._callEdge);
    }
}

DIY_KBH::keyBind_handler::~keyBind_handler() {

}

size_t DIY_KBH::keyBind_handler::size() const {
    return this->_keyBinds.size();
}

int DIY_KBH::keyBind_handler::add(keyBind_handler_unit _newKBH) {

}
int DIY_KBH::keyBind_handler::add(std::string _label, std::vector<int> _keys, _funcDef _callFunc, bool _ordImportant, bool _isolImportant, bool _callEdge) {

}

