
#include "globals_includes.hpp"


void gNC::_menu__node_details(
    gNC::gNODE* toDetail
) {
    if(toDetail==nullptr) return;

    static ImGuiWindowFlags win_flags = 0;
    win_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    win_flags |= ImGuiWindowFlags_NoMove;
    win_flags |= ImGuiWindowFlags_NoCollapse;

    static ImGuiInputTextFlags inpBuff_flags_title  = 0;
    static ImGuiInputTextFlags inpBuff_flags_desc   = 0;
    static ImGuiInputTextFlags inpBuff_flags_bodyT  = 0;

    static ImGuiStyleVar inputText_flags_title  = 0;


    static char inpBuff_title[256];
    static char inpBuff_desc[256];
    static char inpBuff_bodyT[256];

    snprintf(inpBuff_title, IM_ARRAYSIZE(inpBuff_title), "%s", toDetail->label.c_str());
    snprintf(inpBuff_desc,  IM_ARRAYSIZE(inpBuff_desc),  "%s", toDetail->desc.c_str());
    snprintf(inpBuff_bodyT, IM_ARRAYSIZE(inpBuff_bodyT), "%s", toDetail->bodyText.c_str());



    if(ImGui::Begin(("Node details: "+toDetail->addr).c_str(), NULL, win_flags)) {
        ImGui::SetWindowSize(dim__menu__node_detail);

        ImGui::PushID("_title");
        // ImGui::SetCursorPosY(dim__menu__node_detail.y*0.01);
        ImGui::PushItemWidth(dim__menu__node_detail[0]-20);
        ImGui::InputText("", inpBuff_title, sizeof(inpBuff_title), inpBuff_flags_title);
        ImGui::PopItemWidth();
        ImGui::PopID();
    
        ImGui::PushID("_desc");
        // ImGui::SetCursorPosY(dim__menu__node_detail.y*0);
        ImGui::PushItemWidth(dim__menu__node_detail[0]-20);
        ImGui::InputText("", inpBuff_desc, sizeof(inpBuff_desc), inpBuff_flags_desc);
        ImGui::PopID();
        
        ImGui::PushID("_bodyText");
        // ImGui::SetCursorPosY(dim__menu__node_detail.y*0.2);
        ImGui::PushItemWidth(dim__menu__node_detail[0]-20);
        ImGui::InputText("", inpBuff_bodyT, sizeof(inpBuff_bodyT), inpBuff_flags_bodyT);
        ImGui::PopID();

        // ImGui::PushStyleVar(inputText_flags_title, ImVec2(dim__menu__node_detail[0], 200));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(10, 10, 10, 255));
        ImGui::BeginChild("test", ImVec2(dim__menu__node_detail[0], 200));

        ImGui::EndChild();
        ImGui::PopStyleColor();
        // ImGui::PopStyleVar();
        
        ImGui::End();
    }

    toDetail->label = inpBuff_title;
    toDetail->desc  = inpBuff_desc;
    toDetail->bodyText = inpBuff_bodyT;

}
