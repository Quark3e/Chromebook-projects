
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

inline std::vector<int>* update_keys(
    std::vector<int>* ptr_pressed_key = nullptr,
    size_t* ptr_num_keys_pressed = nullptr
) {
    static std::vector<int> pressed_keys;
    static size_t num_keys_pressed = 0;
    ImGuiKey start_key = (ImGuiKey)0;

    pressed_keys.clear();
    for(ImGuiKey key=start_key; key<ImGuiKey_NamedKey_END; key=(ImGuiKey)(key+1)) {
        if(IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue;
        pressed_keys.push_back(key);
    }
    num_keys_pressed = pressed_keys.size();
    // *ptr_pressed_key = pressed_keys;
    // *ptr_num_keys_pressed = num_keys_pressed;

    return &pressed_keys;
}

inline std::vector<int>* update_mouse() {
    static std::vector<int> pressed_mouse;
    static size_t num_mouse_pressed = 0;
    ImGuiMouseButton start_mouse = (ImGuiMouseButton)0;

    pressed_mouse.clear();
    
    return &pressed_mouse;
}

inline bool isKeyPressed(int keyID, std::vector<int>* pressed_keys) {
    if(pressed_keys->size() > 0 && searchVec<int>(*pressed_keys, keyID) != -1)
        return true;
    return false;
}

template<typename addrType>
inline std::string ptrToStr(addrType toConv) {
    const void *address = static_cast<const void*>(toConv);
    std::stringstream ss;
    ss << address;
    return ss.str();
}

#endif