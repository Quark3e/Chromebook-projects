
#include "includes.hpp"
#include "global_variables.hpp"


int main(int argc, char** argv) {
    GUINC::win_dim = ImVec2(DIM_PROGRAM__WIN.x, DIM_PROGRAM__WIN.y);
    
    guiwin.init();
    while(guiwin.running()) {
        ImGuiIO io = ImGui::GetIO();
        guiwin.newFrame();
        if(!guiwin.running()) break;


        guiwin.endFrame();

    }

    return 0;
}