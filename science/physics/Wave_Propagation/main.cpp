<<<<<<< HEAD

#include <imguiwin.hpp>

#include <pos2d.hpp>

#include <vector>
#include <cmath>

#include <keyBind_handler.hpp>
#include <nodeChart/extra_imgui.hpp>


#include <useful.hpp>

#define unit(val) (val)/abs(val)


guiwin_nc::imguiwin guiwin(false);


void _keyBind__Master_Close() {
    exit(1);
}
DIY_KBH::keyBind_handler keyBinds({
    {"Master Close",    {ImGuiKey_ReservedForModCtrl, ImGuiKey_LeftCtrl, ImGuiKey_W}, _keyBind__Master_Close},
    {"MouseLeft",       {ImGuiKey_MouseLeft}},
    {"MouseRight",      {ImGuiKey_MouseRight}}
});
pressed_key__struct guiKeys;



int main(int argc, char** argv) {



    return 0;
}


=======

#include <imguiwin.hpp>
#include <pos2d.hpp>

#include <vector>
#include <cmath>

#include <keyBind_handler.hpp>
#include <nodeChart/extra_imgui.hpp>

#include <useful.hpp>

#define unit(val) (val)/abs(val)




int main(int argc, char** argv) {



    return 0;
}
>>>>>>> 6c02d61 (some minor changes + got imguiwin to build/compile successfully on chromeos unit for both projects nad hte lib itself.)
