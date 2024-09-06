
#include "globals_includes.hpp"


void gNC::_menu__node_details(
    gNC::gNODE* toDetail
) {
    if(toDetail==nullptr) return;

    static ImGuiWindowFlags win_flags = 0;
    win_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    // win_flags |= ImGuiWindowFlags_NoMove;
    win_flags |= ImGuiWindowFlags_NoCollapse;

    static ImGuiInputTextFlags inpBuff_flags_title  = 0;
    static ImGuiInputTextFlags inpBuff_flags_desc   = 0;
    static ImGuiInputTextFlags inpBuff_flags_bodyT  = 0;


    static ImGuiStyleVar inputText_flags_title  = 0;


    static char inpBuff_title[256];
    static char inpBuff_desc[256];
    static char inpBuff_bodyT[1024 * 16];


    if(ImGui::Begin(("Node details: "+toDetail->addr).c_str(), NULL, win_flags)) {
        ImGui::SetWindowPos(ImGui::GetWindowPos());

        ImGui::SetWindowSize(dim__menu__node_detail);

        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::InputText("##_title", &(toDetail->label), inpBuff_flags_title);
    

        ImGui::Separator();
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::InputText("##_desc", &(toDetail->desc), inpBuff_flags_desc);
        
        ImGui::Separator();
        ImGui::InputTextMultiline("##_bodyText", &(toDetail->bodyText), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight()*10), inpBuff_flags_bodyT);


        // ImGui::PushStyleVar(inputText_flags_title, ImVec2(dim__menu__node_detail[0], 200));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(10, 10, 10, 255));
        ImGui::BeginChild("test", ImVec2(dim__menu__node_detail[0], 200));

        ImGui::EndChild();
        ImGui::PopStyleColor();
        // ImGui::PopStyleVar();
        
        ImGui::End();
    }

}


void gNC::_menu__rightClick_default() {


    if(ImGui::BeginMenu("RightClick menu")) {

        ImGui::MenuItem("test", NULL, false, false);
        if(ImGui::MenuItem("New")) {}
        if(ImGui::MenuItem("Open", "Ctrl+O")) {}
        if(ImGui::BeginMenu("Open Recent")) {
            if(ImGui::MenuItem("sub0")) { std::cout << " sub0 "; }
            if(ImGui::MenuItem("sub1")) { std::cout << " sub1 "; }
            if(ImGui::BeginMenu("More..")) {
                if(ImGui::MenuItem("subsub0")) { std::cout << " subsub0 "; }
                if(ImGui::MenuItem("subsub1")) { std::cout << " subsub1 "; }
                if(ImGui::MenuItem("subsub2")) { std::cout << " subsub2 "; }
                ImGui::EndMenu();
            }
        }
        if(ImGui::MenuItem("Save", "Ctrl+S")) {}
        if(ImGui::MenuItem("Save As..")) {}

        


        ImGui::EndMenu();
    }

}

