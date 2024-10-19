
#pragma once
#ifndef HPP_EXTRA_IMGUI
#define HPP_EXTRA_IMGUI


#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>

#include <vector>
#include <list>
#include <HC_useful/useful.hpp>



inline bool IsLegacyNativeDupe(ImGuiKey key) {
    return key >= 0 && key < 512 && ImGui::GetIO().KeyMap[key] != -1;
}

// /**
//  * @brief update `pressed_keys` std::vector<std::vector<int>> container with the keys that has been pressed with `ImGuiKey`
//  * 
//  * @return std::vector<std::vector<int>*> of the local static container.
//  */
// inline std::vector<std::vector<int>>* update_keys() {
//     static int maxSize_history_pressed_keys = 2;
//     static std::vector<std::vector<int>> pressed_keys;
//     static size_t num_keys_pressed = 0;
//     ImGuiKey start_key = (ImGuiKey)0;
//     if(pressed_keys.size()>=maxSize_history_pressed_keys ) {
//         // pressed_keys.clear();
//         for(size_t i=1; i<pressed_keys.size(); i++) {
//             pressed_keys[i-1] = pressed_keys[i];
//         }
//     }
//     else {
//         pressed_keys.push_back(std::vector<int>());
//     }
//     pressed_keys[pressed_keys.size()-1].clear();
//     for(ImGuiKey key=start_key; key<ImGuiKey_NamedKey_END; key=(ImGuiKey)(key+1)) {
//         if(IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue;
//         pressed_keys[pressed_keys.size()-1].push_back(key);
//     }
//     num_keys_pressed = pressed_keys[pressed_keys.size()-1].size();
//     return &pressed_keys;
// }

inline std::vector<int>* update_mouse() {
    static std::vector<int> pressed_mouse;
    static size_t num_mouse_pressed = 0;
    ImGuiMouseButton start_mouse = (ImGuiMouseButton)0;

    pressed_mouse.clear();
    
    return &pressed_mouse;
}


struct pressed_key__struct {
    int maxSize_history_pressed_keys = 20;
    static std::vector<std::vector<int>> pressed;
    static std::vector<std::chrono::_V2::steady_clock::time_point> timePoints;
    size_t num_keys_pressed = 0;

    void    update();
    /**
     * @brief Get the time period between two presses (in pressed keys history) for a given key
     * 
     * @param keyID the id/integer-id of the key to find the time "period" of
     * @param mustAlone whether the key has to be pressed alone for it to be counted as a valid instance
     * @param blankFrame how many frames/iterations in key history `pressed` must not contain the searched `keyID`
     * @param msLim minimum millisecond limit for the period to return the value (recommended to 200ms for a double click)
     * @return float milliseconds for the period
     */
    float   keyPeriod(int keyID, bool mustAlone=false, int blankFrame=1, float msLim=200);
};
inline pressed_key__struct guiKeys;


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





#endif