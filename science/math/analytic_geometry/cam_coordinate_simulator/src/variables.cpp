
#include "variables.hpp"



pos2d<int> dim__programWindow{DEFAULT__WINDOW_WIDTH, DEFAULT__WINDOW_HEIGHT};
pos2d<int> dim__draw_area(dim__programWindow);

pos2d<int> pos__draw_area{0, 0};


GUINC::imguiwin guiwin(false, dim__programWindow.x, dim__programWindow.y);

void _keyBind__Master_Close() {
    // exit(1);
    guiwin.exit();
}

DIY_KBH::keyBind_handler keyBinds({
    {"Master Close",    {ImGuiKey_ReservedForModCtrl, ImGuiKey_LeftCtrl, ImGuiKey_W}, _keyBind__Master_Close},
    {"MouseLeft",       {ImGuiKey_MouseLeft}},
    {"MouseRight",      {ImGuiKey_MouseRight}},
    {"Escape",          {ImGuiKey_Escape}, _keyBind__Master_Close}
});
pressed_key__struct guiKeys;


size_t selectable_itemID = 0;
int selectable_isSelected = -1;


SOC::CamSystem camSys({500, 200}, {
    {90,  75, 300, 500},
    {90, 110, 700, 500}
});

