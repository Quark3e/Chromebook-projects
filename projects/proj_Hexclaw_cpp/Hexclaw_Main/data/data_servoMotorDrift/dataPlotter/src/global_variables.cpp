
#include "global_variables.hpp"


GUINC::imguiwin guiwin(false);

void _keyBind__Master_Close() {

}
DIY_KBH::keyBind_handler keyBinds({
    {"Master Close",    {ImGuiKey_ReservedForModCtrl, ImGuiKey_LeftCtrl, ImGuiKey_W}, _keyBind__Master_Close},
    {"MouseLeft",       {ImGuiKey_MouseLeft}},
    {"MouseRight",      {ImGuiKey_MouseRight}}
});
pressed_key__struct guiKeys;

int keyAction_mouseLeft = -1;

pos2d<int> DIM_PROGRAM__WIN(1000, 1000);


pos2d<int> DIM_PROGRAM__MOTOR_SELECT_PANEL(200, 1000);


