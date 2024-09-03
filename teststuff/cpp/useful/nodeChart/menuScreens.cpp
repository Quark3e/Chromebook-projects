
#include "globals_includes.hpp"


void gNC::_menu__node_details(
    gNC::gNODE* toDetail
) {
    if(toDetail==nullptr) return;

    static ImGuiWindowFlags win_flags = 0;
    win_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    win_flags |= ImGuiWindowFlags_NoMove;
    win_flags |= ImGuiWindowFlags_NoCollapse;

    static ImGuiInputFlags inpBuff_flags_title  = 0;
    static ImGuiInputFlags inpBuff_flags_desc   = 0;
    static ImGuiInputFlags inpBuff_flags_bodyT  = 0;

    static char inpBuff_title[256];
    static char inpBuff_desc[256];
    static char inpBuff_bodyT[256];


    if(ImGui::Begin(("Node details: "+toDetail->addr).c_str(), NULL, win_flags)) {
        ImGui::SetWindowSize(dim__menu__node_detail);
        
        ImGui::BeginChild("Title");
        ImGui::PushID("_title");
        ImGui::InputText("", inpBuff_title, sizeof(inpBuff_title), inpBuff_flags_title);
        ImGui::PopID();
        ImGui::EndChild();
    
        ImGui::BeginChild("Description");
        ImGui::PushID("_desc");
        ImGui::InputText("", inpBuff_desc, sizeof(inpBuff_desc), inpBuff_flags_desc);
        ImGui::PopID();
        ImGui::EndChild();
        
        ImGui::BeginChild("BodyText");
        ImGui::PushID("_bodyText");
        ImGui::InputText("", inpBuff_bodyT, sizeof(inpBuff_bodyT), inpBuff_flags_bodyT);
        ImGui::PopID();
        ImGui::EndChild();
        
        ImGui::End();
    }


}
