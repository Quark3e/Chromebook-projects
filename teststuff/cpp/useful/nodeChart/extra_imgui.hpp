
#pragma once
#ifndef HPP_EXTRA_IMGUI
#define HPP_EXTRA_IMGUI


#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>

#include <vector>
#include <list>
#include <useful.hpp>
#include <diy_dictionary.hpp>

#include <keyBind_handler.hpp>


struct pressed_key__struct {
    int maxSize_history_pressed_keys = 200;
    static std::vector<std::vector<int>> pressed;
    static std::vector<std::chrono::steady_clock::time_point> timePoints;
    size_t num_keys_pressed = 0;

    /**
     * @brief Main update function for updating the struct's `::pressed` history container with current frames keys
     * 
     */
    void update();

    std::vector<int> holding_keys;      // Container to hold the keys that are currently being held
    int holding_keys__allowed_gaps = 2; // Number of history instances in a row that are allowed to not be the holding key for the holding key to still be registered as "holding".
    int holding_keys__holdingLim = 10;  // Minimum limit number that a key must be active for it to be registered as "holding"
    /**
     * A helper dictionary used for knowing how many times an ImGuiKey has occurred.
     *  the correlated integer vlaue is either >0 or <0:
     *   - `<0`  key has occurred but there has been the integer number of history instances without that key. If it's less than ``holding_keys__allowed_gaps`` then it'll be erased from this dict.
     *   - `>0`  key has occurred the integer value number of history instances. If this is more or equal to `holding_keys__holdingLim` then it'll be added to `holding_keys` vector
     */
    DIY::typed_dict<int, int> __refDict_holding_keys_occur;
    /**
     * Helper directory that holds the direction of the latest value change in `__refDict_holding_keys_occur` for given keys.
     *  - `<0`  newest key value is smaller than previous
     *  - `=0`  newest key value is same as previous
     *  - `>0`  newest key value is bigger than previous
     */
    DIY::typed_dict<int, int> __refDict_keyChangeDir;
    void __update_holding_keys();
    /**
     * @brief Check whether an ImGuiKey is *registered as* "holding down" in accordance with member `holding_keys__` settings.
     * 
     * @param _key ImGuiKey or an integer for the key to check
     * @return true if the key is held down.
     * @return false if the key isn't held down.
     */
    bool isHolding(int _key);

    std::vector<int> clicked_keys;      // Container to hold the keys that are currently registered as "clicked" on both the rising- and falling state change. To determine whether the key has been clicked or is lifted, check __refDict_keyChangeDir.get(_key) value or use ::isClicked(int) member.
    int clicked_keys__allowed_gaps = 2; // Maximum number of history instances in a row that are allowed to not contain the key to still not update the `clicked_keys_...` containers.
    void __update_clicked_keys();
    int isClicked(int _key);

    /**
     * @brief Get the time period between two presses (in pressed keys history) for a given key
     * 
     * @param keyID the id/integer-id of the key to find the time "period" of
     * @param mustAlone whether the key has to be pressed alone for it to be counted as a valid instance
     * @param blankFrame how many frames/iterations in key history `pressed` must not contain the searched `keyID`
     * @param msLim minimum millisecond limit for the period to return the value (recommended to 200ms for a double click)
     * @return float milliseconds for the period
     */
    float keyPeriod(int keyID, bool mustAlone=false, int blankFrame=1, float msLim=200);
};

inline std::vector<std::vector<int>> pressed_key__struct::pressed;
inline std::vector<std::chrono::steady_clock::time_point> pressed_key__struct::timePoints;

inline void pressed_key__struct::update() {
    // ImGuiKey start_key = (ImGuiKey)0;
    auto timeNow = std::chrono::steady_clock::now();
    if(pressed.size()>0 && timeNow == timePoints.at(timePoints.size()-1)) return;
    
    if(pressed.size()>=maxSize_history_pressed_keys) {
        for(size_t i=1; i<pressed.size(); i++) {
            pressed[i-1]    = pressed[i];
            timePoints[i-1] = timePoints[i];
        }
        timePoints[timePoints.size()-1] = timeNow;
    }
    else {
        pressed.push_back(std::vector<int>());
        timePoints.push_back(timeNow);
    }
    pressed[pressed.size()-1].clear();
    for(ImGuiKey key=ImGuiKey_NamedKey_BEGIN; key<ImGuiKey_NamedKey_END; key=(ImGuiKey)(key+1)) {
        if(/*IsLegacyNativeDupe(key) || */!ImGui::IsKeyDown(key)) continue;
        pressed[pressed.size()-1].push_back(key);
    }
    num_keys_pressed = pressed[pressed.size()-1].size();

    this->__update_holding_keys();
    this->__update_clicked_keys();
}

inline void pressed_key__struct::__update_holding_keys() {
    static std::chrono::steady_clock::time_point _lastChecked = std::chrono::steady_clock::now();

    if(pressed.size()==0 || timePoints.size()==0) return;
    if(_lastChecked==this->timePoints.at(timePoints.size()-1)) return;
    
    std::vector<int> &_recentPressed = pressed.at(pressed.size()-1);

    /// Check the elements in __refDict_holding_keys_occur with new keys
    for(size_t i=0; i<__refDict_holding_keys_occur.size(); i++) {
        int idxFound = -1;
        for(size_t ii=0; ii<_recentPressed.size(); ii++) {
            if(_recentPressed.at(ii)==__refDict_holding_keys_occur.getKey(i)) {
                __refDict_holding_keys_occur[i] += (__refDict_holding_keys_occur[i]==-1? 2 : 1);
                __refDict_keyChangeDir[i] = 1;
                idxFound = ii;
                break;
            }
        }
        if(idxFound==-1) {
            __refDict_holding_keys_occur[i] -= (__refDict_holding_keys_occur[i]==1? 2 : 1);
            __refDict_keyChangeDir[i] = -1;
        }
    }

    /// Check the elements in _recentPressed to add new keys to the refDict: Only checks elements that are only in _recentPressed, i.e set theory: _recentPressed-__refDict_holding_keys_occur
    int foundIdx = -1;
    for(size_t i=0; i<_recentPressed.size(); i++) {
        foundIdx = -1;
        if((foundIdx = __refDict_holding_keys_occur.find((ImGuiKey)_recentPressed.at(i), false)) < 0) {
            __refDict_holding_keys_occur.add((ImGuiKey)_recentPressed.at(i), 1);
            __refDict_keyChangeDir.add(_recentPressed.at(i), 1);
        }
    }

    /// Go through the value of each item in dict and fix them
    for(size_t i=0; i<__refDict_holding_keys_occur.size(); i++) {
        int *refDictVal_ptr = &__refDict_holding_keys_occur[i];
        int found = -1;
        for(size_t ii=0; ii<this->holding_keys.size(); ii++) {
            if(__refDict_holding_keys_occur.getKey(i)==this->holding_keys.at(ii)) {
                found = ii;
                break;
            }
        }

        if(*refDictVal_ptr + holding_keys__allowed_gaps >= holding_keys__holdingLim) { // refDictVal_ptr value is "above" threshold
            if(*refDictVal_ptr>holding_keys__holdingLim) { // refDictVal_ptr value "reached the ceiling"
                *refDictVal_ptr = holding_keys__holdingLim;
                __refDict_keyChangeDir[i] = 0;
            }
            else {
                *refDictVal_ptr++;
            }
            if(found<0) holding_keys.push_back(__refDict_holding_keys_occur.getKey(i)); 
        }
        else {
            if(found>-1) {
                // (*refDictVal_ptr)++;
                auto itr = holding_keys.begin();
                std::advance(itr, found);
                holding_keys.erase(itr);
            }
            else {
                // (*refDictVal_ptr)--;
            }
            if(*refDictVal_ptr < 0) {
                __refDict_holding_keys_occur.eraseIdx(i);
                __refDict_keyChangeDir.eraseIdx(i);
                i--;
            }
        }
    }

    // std::cout << DIY::prettyPrint_vec1<int>(_recentPressed) << " | ";
    // std::cout << __refDict_holding_keys_occur << " | ";
    // std::cout << DIY::prettyPrint_vec1<int>(this->holding_keys) << std::endl;

    _lastChecked = this->timePoints.at(timePoints.size()-1);
}
inline bool pressed_key__struct::isHolding(int _key) {
    for(size_t i=0; i<this->holding_keys.size(); i++) {
        if(this->holding_keys.at(i)==_key) return true;
    }
    return false;
}

inline void pressed_key__struct::__update_clicked_keys() {
    static auto _lastChecked = std::chrono::steady_clock::now();

    if(pressed.size()==0 || timePoints.size()==0) return;
    if(_lastChecked==this->timePoints.at(timePoints.size()-1)) return;

    this->__update_holding_keys();
    
    for(size_t i=0; i<__refDict_keyChangeDir.size(); i++) {
        if(__refDict_holding_keys_occur.get(__refDict_keyChangeDir.getKey(i))==(holding_keys__holdingLim-clicked_keys__allowed_gaps)) {
            bool found=false;
            for(size_t ii=0; ii<clicked_keys.size(); ii++) {
                if(clicked_keys.at(ii)==__refDict_keyChangeDir.getKey(i)) {
                    found = true;
                    break;
                }
            }
            if(!found) clicked_keys.push_back(__refDict_keyChangeDir.getKey(i));
        }
        else {
            for(size_t ii=0; ii<clicked_keys.size(); ii++) {
                if(clicked_keys.at(ii)==__refDict_keyChangeDir.getKey(i)) {
                    auto itr = clicked_keys.begin();
                    std::advance(itr, ii);
                    clicked_keys.erase(itr);
                    break;
                }
            }
        }
    }


    _lastChecked = this->timePoints.at(timePoints.size()-1);
}
inline int  pressed_key__struct::isClicked(int _key) {
    int _retur = 2;
    for(size_t i=0; i<this->clicked_keys.size(); i++) {
        if(this->clicked_keys.at(i)==_key) {
            _retur = __refDict_keyChangeDir.get(_key);
        }
    }
    return _retur;
}

inline float pressed_key__struct::keyPeriod(int keyID, bool mustAlone, int blankFrame, float msLim) {
    if(pressed.size()==0) return -1;
    float period = -1;

    std::chrono::steady_clock::time_point _t1, _t2;

    int found1 = -1;
    int found2 = -1;
    for(int i=pressed.size()-1; i>=0; i--) {
        for(auto _key : pressed[i]) {
            if(_key==keyID && (!mustAlone || (mustAlone && pressed[i].size()==1))) {
                if(found1 != -1 && abs(i-found1) > blankFrame) {
                    found2 = i;
                    _t2 = timePoints[i];
                }
                else {
                    found1 = i;
                    _t1 = timePoints[i];
                }
            }
        }
        if(found2>-1) {
            period = std::chrono::duration_cast<std::chrono::milliseconds>(_t1-_t2).count();
            return (period<=msLim? period : -1);
        }
    }
    return period;
}



/**
 * @brief Check if a single key is pressed (includes mouse buttons). i.e check if container contains a certain value.
 * 
 * @param keyID the integer identifier for the key to look for
 * @param pressed_keys pointer to `std::vector<int>` container that holds the key ID's
 * @return true if `keyID` is pressed
 * @return false if `keyID` is NOT pressed
 * @note Common keys:
 * - Mouse_left:    655
 * - Mouse_right:   656
 */
inline bool isKeyPressed(int keyID, std::vector<int>* pressed_keys) {
    if(pressed_keys->size() > 0 && searchVec<int>(*pressed_keys, keyID) != -1)
        return true;
    return false;
}
inline bool isKeyPressed(int keyID, std::vector<std::vector<int>>* pressed_keys_history, int history_idx=-1) {
    size_t _history_size = pressed_keys_history->size();
    assert(_history_size > 0);
    assert(history_idx < static_cast<int>(_history_size));
    assert(abs(history_idx) < _history_size);

    if(history_idx < 0) history_idx = static_cast<int>(_history_size) + history_idx;

    return isKeyPressed(keyID, &(pressed_keys_history->operator[](history_idx)));
}

inline bool areKeysPressed(std::vector<int> keyIDs, std::vector<std::vector<int>>* pressed_keys_history, int history_idx=-1) {
    size_t _history_size = pressed_keys_history->size();
    assert(_history_size > 0);
    assert(history_idx < static_cast<int>(_history_size));
    assert(abs(history_idx) < _history_size);
    assert(keyIDs.size()>0);

    if(history_idx < 0) history_idx = static_cast<int>(_history_size) + history_idx;
    std::vector<int>* keysVec = &(pressed_keys_history->operator[](history_idx));

    if(keysVec->size() < keyIDs.size()) return false;

    int matchCount = 0;

    for(int _key : keyIDs) {
        if(isKeyPressed(_key, keysVec)) matchCount++;
    }

    return (matchCount == static_cast<int>(keyIDs.size())? true : false);
}

/**
 * @brief Convert pointer address to std::string
 * 
 * @tparam addrType type of the address
 * @param toConv the address to convert
 * @return std::string of the address
 */
template<typename addrType>
inline std::string ptrToStr(addrType toConv) {
    const void *address = static_cast<const void*>(toConv);
    std::stringstream ss;
    ss << address;
    return ss.str();
}

/**
 * @brief Check if the `cursor` position is inside region noted by the bounding box corners
 * 
 * @param cursor position to check relative to the bounding box
 * @param pos_topLeft 2d coordinates of the top-left corner of the region bounding box
 * @param pos_botRight 2d coordinates of the bottom-right corner of the region bounding box
 * @return true if `cursor` coordinate is within the regions bounding box.
 * @return false if `cursor` coordinate is outside the regions bounding box.
 */
inline bool inRegion(
    ImVec2 cursor,
    ImVec2 pos_topLeft,
    ImVec2 pos_botRight
) {
    if(
        (cursor.x > pos_topLeft.x && cursor.x < pos_botRight.x) &&
        (cursor.y > pos_topLeft.y && cursor.y < pos_botRight.y)
    ) return true;
    return false;
}

inline bool validExtension(
    std::string checkStr,
    std::vector<std::string> validExt
) {
    std::string f_ext = "";
    size_t dotPos = checkStr.find('.');

    //check if file extension of the selected file is valid according to _valid__extensions
    if(dotPos==std::string::npos) { //filename doesn't have an extension
        for(auto ext : validExt) {
            if(ext=="") {
                return true;
            }
        }
    }
    else {
        for(size_t i=dotPos+1; i<checkStr.length(); i++) f_ext+=checkStr[i];

        for(auto ext : validExt) {
            if(ext==f_ext) {
                return true;
            }
        }
    }
    return false;
}

template<typename _varType> inline int checkExistence(_varType toFind, std::initializer_list<_varType> toSearch) {
    int count=0;
    for(auto itr=toSearch.begin(); itr!=toSearch.end(); ++itr) {
        if(*itr==toFind) return count;
        count++;
    }
    return -1;
}
template<typename _varType> inline int checkExistence(_varType toFind, const std::vector<_varType>& toSearch) {
    for(size_t i=0; i<toSearch.size(); i++) {
        if(toSearch.at(i)==toFind) return static_cast<int>(i);
    }
    return -1;
}
template<typename _varType> inline int checkExistence(_varType toFind, const std::list<_varType>& toSearch) {
    int count=0;
    for(auto itr=toSearch.begin(); itr!=toSearch.end(); ++itr) {
        if(*itr==toFind) return count;

        count++;
    }

    return -1;
}
template<typename _varType> inline int checkExistence(_varType toFind, _varType toSearch[], int arrLen) {
    for(int i=0; i<arrLen; i++) {
        if(toSearch[i]==toFind) return i;
    }
    return -1;
}


inline ImVec2 ImVec2_add(ImVec2 _a, ImVec2 _b) {
    return ImVec2(_a.x+_b.x, _a.y+_b.y);
}
inline ImVec2 ImVec2_subtract(ImVec2 _a, ImVec2 _b) {
    return ImVec2(_a.x-_b.x, _a.y-_b.y);
}
inline ImVec2 ImVec2_multiply(ImVec2 _a, ImVec2 _b) {
    return ImVec2(_a.x*_b.x, _a.y*_b.y);
}
inline ImVec2 ImVec2_divide(ImVec2 _a, ImVec2 _b) {
    return ImVec2(_a.x/_b.x, _a.y/_b.y);
}


#endif