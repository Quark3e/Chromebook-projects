
#include "globals_includes.hpp"


bool gNC::_winFocused__node_details = false;
bool gNC::_winFocused__link_details = false;


void gNC::_menu__node_details(
    gNC::gNODE* toDetail
) {
    assert(toDetail!=nullptr);
    static gNC::gNODE* init_node = nullptr;


    static ImGuiWindowFlags win_flags = 0;
    win_flags |= ImGuiWindowFlags_NoResize;
    win_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    win_flags |= ImGuiWindowFlags_NoMove;
    win_flags |= ImGuiWindowFlags_NoCollapse;
    win_flags |= ImGuiWindowFlags_HorizontalScrollbar;
    // win_flags |= ImGuiWindowFlags_Verti

    static ImGuiInputTextFlags inpText_flags_label  = 0;
    static ImGuiInputTextFlags inpText_flags_desc   = 0;
    static ImGuiInputTextFlags inpText_flags_bodyT  = 0;


    static ImGuiTableFlags node_links_table_flags = 0;
    node_links_table_flags |= ImGuiTableFlags_Borders;
    node_links_table_flags |= ImGuiTableFlags_SizingFixedSame;
    node_links_table_flags |= ImGuiTableFlags_SizingFixedFit;

    
    static int _win_widthOffset = 20;

    if(ImGui::Begin((" node: "+toDetail->addr).c_str(), NULL, win_flags)) {
        // std::cout <<std::boolalpha << ImGui::IsWindowFocused()<<" | ";
        if(init_node!=toDetail) {
            // ImGui::SetWindowFocus((" node: "+toDetail->addr).c_str());
        }
        _winFocused__node_details = ImGui::IsWindowFocused();
        // ImGui::SetWindowPos(ImGui::GetWindowPos());
        ImGui::SetWindowPos(dim__menu__detail__offset);
        if(linkPtr_menu__link_details) {
            ImGui::SetWindowSize(ImVec2(dim__menu__detail[0], dim__menu__detail[1]*0.5));
        }
        else {
            ImGui::SetWindowSize(dim__menu__detail);
        }
        // }

        ImGui::PushItemWidth(dim__menu__detail.x - _win_widthOffset - 110);
        ImGui::InputText("##_title", &(toDetail->label), inpText_flags_label);
        
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DateChooser(("##"+ptrToStr<gNC::gNODE*>(toDetail)+"_date").c_str(), toDetail->date);

        ImGui::Separator();
        ImGui::PushItemWidth(dim__menu__detail.x-_win_widthOffset);
        ImGui::InputText("##_desc", &(toDetail->desc), inpText_flags_desc);
        
        ImGui::Separator();
        ImGui::InputTextMultiline("##_bodyText", &(toDetail->bodyText), ImVec2(dim__menu__detail.x-_win_widthOffset, ImGui::GetTextLineHeight()*5), inpText_flags_bodyT);


        ImGui::BeginGroup();
        if(ImGui::BeginChild("##window_node_links", ImVec2(-FLT_MIN, 0))) {
            if(ImGui::BeginTable("node_links", 3, node_links_table_flags, ImVec2(dim__menu__detail.x-_win_widthOffset, 0))) {
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Link");
                ImGui::TableSetupColumn("Node");
                ImGui::TableHeadersRow();
                
                for(int v=0; v<4; v++) {
                    std::vector<gNC::gLINK*>& vecRef = (v==0? toDetail->ln_in : (v==1? toDetail->ln_out : (v==2? toDetail->ln_add : toDetail->ln_share)));
                    ImGui::TableNextRow();
                    
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text((v==0? "in": (v==1? "out" : (v==2? "add" : "share"))));
                    // ImGui::TableSetColumnIndex(1);
                    ImGui::TableSetColumnIndex(1);
                    for(auto _link : vecRef) {
                        if(ImGui::Button((_link? ptrToStr<gNC::gLINK*>(_link).c_str() : "nullptr"))) {
                            linkPtr_menu__link_details = _link;
                        }
                        // ImGui::Text((_link==nullptr? "nullptr" : ptrToStr<gNC::gLINK*>(_link).c_str()));
                    }
                    ImGui::TableSetColumnIndex(2);
                    for(auto _link : vecRef) {
                        gNC::gNODE* _printPtr = (v%2==0? _link->src : _link->dest);
                        if(ImGui::Button((_printPtr? ptrToStr<gNC::gNODE*>(_printPtr).c_str() : "nullptr"))) {
                            nodePtr_menu__node_details = _printPtr;
                            if(_printPtr) {
                                ImGui::SetWindowFocus(_printPtr->addr.c_str());
                            }
                        }
                        // ImGui::Text((_printPtr==nullptr? "nullptr" : ptrToStr<gNC::gNODE*>(_printPtr).c_str()));
                    }
                }

                ImGui::EndTable();
            }
            ImGui::EndChild();
        }
        ImGui::EndGroup();
        
        ImGui::End();
    }

    if(init_node!=toDetail) init_node = toDetail;
}

void gNC::_menu__link_details(
    gNC::gLINK* toDetail
) {
    assert(toDetail!=nullptr);
    static gNC::gLINK* init_link = nullptr;


    static ImGuiWindowFlags win_flags = 0;
    win_flags |= ImGuiWindowFlags_NoResize;
    win_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    win_flags |= ImGuiWindowFlags_NoMove;
    win_flags |= ImGuiWindowFlags_NoCollapse;
    win_flags |= ImGuiWindowFlags_HorizontalScrollbar;


    static ImGuiInputTextFlags inpText_flags_label  = 0;
    static ImGuiInputTextFlags inpText_flags_desc   = 0;
    static ImGuiInputTextFlags inpText_flags_bodyT  = 0;


    static ImGuiTableFlags links_table_flags = 0;
    links_table_flags |= ImGuiTableFlags_Borders;
    links_table_flags |= ImGuiTableFlags_SizingFixedSame;
    links_table_flags |= ImGuiTableFlags_SizingFixedFit;


    static int _win_widthOffset = 20;

    if(ImGui::Begin((" link: "+toDetail->addr).c_str(), NULL, win_flags)) {
        if(toDetail != init_link) {
            // ImGui::SetWindowFocus((" link: "+toDetail->addr).c_str());
        }
        _winFocused__link_details = ImGui::IsWindowFocused();
        // ImGui::SetWindowPos(ImGui::GetWindowPos());
        if(nodePtr_menu__node_details) {
            ImGui::SetWindowPos(ImVec2(dim__menu__detail__offset.x, dim__menu__detail__offset.y+dim__menu__detail[1]*0.5));
            ImGui::SetWindowSize(ImVec2(dim__menu__detail[0], dim__menu__detail[1]*0.5));
        }
        else {
            ImGui::SetWindowPos(dim__menu__detail__offset);
            ImGui::SetWindowSize(dim__menu__detail);
        }

        ImGui::PushItemWidth(dim__menu__detail.x - _win_widthOffset - 110);
        ImGui::InputText("##_title", &(toDetail->label), inpText_flags_label);

        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DateChooser(("##"+ptrToStr<gNC::gLINK*>(toDetail)+"_date").c_str(), toDetail->date);


        ImGui::Separator();
        ImGui::PushItemWidth(dim__menu__detail.x-_win_widthOffset);
        ImGui::InputText("##_desc", &(toDetail->desc), inpText_flags_desc);

        ImGui::Separator();
        ImGui::InputTextMultiline("##_bodyText", &(toDetail->bodyText), ImVec2(dim__menu__detail.x-_win_widthOffset, ImGui::GetTextLineHeight()*5), inpText_flags_bodyT);


        ImGui::BeginGroup();
        if(ImGui::BeginChild("##window_link_nodes", ImVec2(-FLT_MIN, 0))) {
            if(ImGui::BeginTable("link_nodes", 3, links_table_flags, ImVec2(dim__menu__detail.x-_win_widthOffset, 0))) {
                ImGui::TableSetupColumn("##_LCorner");
                ImGui::TableSetupColumn("Address");
                ImGui::TableSetupColumn("Type");
                ImGui::TableHeadersRow();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("src:");
                ImGui::TableSetColumnIndex(1);
                if(ImGui::Button((toDetail->src? ptrToStr<gNC::gNODE*>(toDetail->src).c_str() : "nullptr"))) {
                    nodePtr_menu__node_details = toDetail->src;
                    if(toDetail->src) {
                        ImGui::SetWindowFocus(toDetail->src->addr.c_str());
                    }
                }
                // ImGui::Text((toDetail->src? ptrToStr<gNC::gNODE*>(toDetail->src).c_str() : "nullptr"));
                ImGui::TableSetColumnIndex(2);
                ImGui::Text((toDetail->type_src==1? "out" : "share"));
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("dest:");
                ImGui::TableSetColumnIndex(1);
                // ImGui::Text((toDetail->dest? ptrToStr<gNC::gNODE*>(toDetail->dest).c_str() : "nullptr"));
                if(ImGui::Button((toDetail->dest? ptrToStr<gNC::gNODE*>(toDetail->dest).c_str() : "nullptr"))) {
                    nodePtr_menu__node_details = toDetail->dest;
                    if(toDetail->dest) {
                        ImGui::SetWindowFocus(toDetail->dest->addr.c_str());
                    }
                }
                ImGui::TableSetColumnIndex(2);
                ImGui::Text((toDetail->type_dest==0? "in" : "add"));


                ImGui::EndTable();
            }
            ImGui::EndChild();
        }
        ImGui::EndGroup();

        ImGui::End();
    }


    if(init_link!=toDetail) init_link = toDetail;
}


void gNC::_menu__rightClick(
    gNC::guiNodeChart* chart
) {
    static bool init = false;

    static ImGuiIO& io = ImGui::GetIO();
    ImVec2 mousePos_temp = io.MousePos;

    if(ImGui::BeginPopup("_menu__rightClick__default")) {

        ImGui::MenuItem("Default menu", NULL, false, false);
        if(ImGui::MenuItem("Create Node")) {
            chart->NODE_create(
                mousePos_temp.x - chart->screen_pos[0],
                mousePos_temp.y - chart->screen_pos[1],
                "", "", ""
            );
            nodePtr_menu__node_details = chart->lastAdded_NODE();
        }
            
        ImGui::EndPopup();
    }
    if(ImGui::BeginPopup("_menu__rightClick__node")) {
        assert(nodePtr_menu__rightClick);
        ImGui::MenuItem("Node menu", NULL, false, false);
        if(ImGui::MenuItem("Delete node")) {
            if(nodePtr_menu__node_details == nodePtr_menu__rightClick) nodePtr_menu__node_details = nullptr;
            chart->NODE_delete(nodePtr_menu__rightClick);
        }
        ImGui::EndPopup();
    }
    if(ImGui::BeginPopup("_menu__rightClick__link")) {
        assert(linkPtr_menu__rightClick);
        ImGui::MenuItem("Link menu", NULL, false, false);
        if(ImGui::MenuItem("Delete link")) {
            if(linkPtr_menu__link_details == linkPtr_menu__rightClick) linkPtr_menu__link_details = nullptr;
            chart->LINK_delete(linkPtr_menu__rightClick);
        }
        ImGui::EndPopup();
    }
    else {
        mouseAction_right = -1;
    }

    // if(ImGui::MenuItem("_menu__rightClick_"))

    if(!init) init=true;
}

