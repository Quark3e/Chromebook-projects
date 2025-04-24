
#include "variables_gui.hpp"

GUINC::imguiwin guiwin(false);


void _keyBind__Master_Close() {
    exit(1);
}

DIY_KBH::keyBind_handler keyBinds({
    {"Master Close",    {ImGuiKey_ReservedForModCtrl, ImGuiKey_LeftCtrl, ImGuiKey_W}, _keyBind__Master_Close},
    {"MouseLeft",       {ImGuiKey_MouseLeft}},
    {"MouseRight",      {ImGuiKey_MouseRight}}
});
pressed_key__struct guiKeys;


