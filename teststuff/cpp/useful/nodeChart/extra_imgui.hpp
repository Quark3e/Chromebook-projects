
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

/**
 * @brief update `pressed_`keys std:.vector<int> container with the keys that has been pressed with `ImGuiKey`
 * 
 * @return std::vector<int>* of the local static container.
 */
inline std::vector<int>* update_keys() {
    static std::vector<int> pressed_keys;
    static size_t num_keys_pressed = 0;
    ImGuiKey start_key = (ImGuiKey)0;

    pressed_keys.clear();
    for(ImGuiKey key=start_key; key<ImGuiKey_NamedKey_END; key=(ImGuiKey)(key+1)) {
        if(IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue;
        pressed_keys.push_back(key);
    }
    num_keys_pressed = pressed_keys.size();

    return &pressed_keys;
}

inline std::vector<int>* update_mouse() {
    static std::vector<int> pressed_mouse;
    static size_t num_mouse_pressed = 0;
    ImGuiMouseButton start_mouse = (ImGuiMouseButton)0;

    pressed_mouse.clear();
    
    return &pressed_mouse;
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


#endif