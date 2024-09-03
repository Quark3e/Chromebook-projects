
#include "menuScreens.hpp"
#include "globals_includes.hpp"


void _menu__node_details(
    gNC::guiNodeChart& _NC,
    gNC::gNODE* toDetail
) {

    static ImGuiWindowFlags win_flags = 0;

    static ImGuiInputFlags inpBuff_flags_title  = 0;
    static ImGuiInputFlags inpBuff_flags_desc   = 0;
    static ImGuiInputFlags inpBuff_flags_bodyT  = 0;

    static char inpBuff_title[256];
    static char inpBuff_desc[256];
    static char inpBuff_bodyT[256];


    if(ImGui::Begin((toDetail->addr+"node details").c_str(), NULL, win_flags)) {
        ImGui::SetWindowSize(dim__menu__node_detail);
        
        ImGui::TextUnformatted(toDetail->addr.c_str());

        if(ImGui::BeginChild("Title")) {
            ImGui::PushID("_title");
            ImGui::InputText("", inpBuff_title, sizeof(inpBuff_title), inpBuff_flags_title);

            ImGui::PopID();
            ImGui::EndChild();
        }
        if(ImGui::BeginChild("Description")) {
            ImGui::PushID("_desc");
            ImGui::InputText("", inpBuff_desc, sizeof(inpBuff_desc), inpBuff_flags_desc);

            ImGui::PopID();
            ImGui::EndChild();
        }
        if(ImGui::BeginChild("BodyText")) {
            ImGui::PushID("_bodyText");
            ImGui::InputText("", inpBuff_bodyT, sizeof(inpBuff_bodyT), inpBuff_flags_bodyT);

            ImGui::PopID();
            ImGui::EndChild();
        }
        ImGui::End();
    }


}
