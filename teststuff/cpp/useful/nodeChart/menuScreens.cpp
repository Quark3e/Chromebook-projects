
#include "globals_includes.hpp"


bool gNC::_winFocused__node_details = false;
bool gNC::_winFocused__link_details = false;

bool gNC::_winHover__timeline = false;

void gNC::_menu__node_details(
    gNC::gNODE* toDetail,
    gNC::guiNodeChart *chart
) {
    assert(toDetail!=nullptr);
    static gNC::gNODE* init_node = nullptr;
    static bool init_prev = false;
    if(init_node) init_prev = true;

    static ImGuiWindowFlags win_flags = 0;
    // win_flags |= ImGuiWindowFlags_NoResize;
    win_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    win_flags |= ImGuiWindowFlags_NoMove;
    win_flags |= ImGuiWindowFlags_NoCollapse;
    win_flags |= ImGuiWindowFlags_HorizontalScrollbar;

    static ImGuiInputTextFlags inpText_flags_label  = 0;
    static ImGuiInputTextFlags inpText_flags_desc   = 0;
    static ImGuiInputTextFlags inpText_flags_bodyT  = 0;


    static ImGuiTableFlags node_links_table_flags = 0;
    node_links_table_flags |= ImGuiTableFlags_Borders;
    node_links_table_flags |= ImGuiTableFlags_SizingFixedSame;
    node_links_table_flags |= ImGuiTableFlags_SizingFixedFit;

    
    static int _win_widthOffset = 20;

    if(linkPtr_menu__link_details) {
        ImGui::SetNextWindowSizeConstraints(
            dim__menu__detail().min,
            ImVec2(dim__menu__detail().max.x, dim__menu__detail().min.y)
        );
    }
    else {
        ImGui::SetNextWindowSizeConstraints(
            dim__menu__detail().min,
            dim__menu__detail().max
        );
    }
    ImGui::SetNextWindowPos(pos__menu__detail__offset, 0, ImVec2(0, 0));
    if(ImGui::Begin((" node: "+toDetail->addr).c_str(), NULL, win_flags)) {
        if(ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
            gNC::_DRAW_SCALAR_SCOLL_LOCK = true;
        }
        // std::cout <<std::boolalpha << ImGui::IsWindowFocused()<<" | ";
        if(init_node!=toDetail) {
            // ImGui::SetWindowFocus((" node: "+toDetail->addr).c_str());
            // init_prev = false;
            // chart->setScreen_pos(toDetail->pos.x, toDetail->pos.y);
            if(linkPtr_menu__link_details) {
                ImGui::SetWindowSize(dim__menu__detail().min);
            }
            else {
                ImGui::SetWindowSize(ImVec2(dim__menu__detail().min.x, dim__menu__detail().min.y));
            }


        }
        else {
            if(!init_prev) mouseAction_left = 1;
        }
        _winFocused__node_details = ImGui::IsWindowFocused();
        // ImGui::SetWindowPos(ImGui::GetWindowPos());
        // ImGui::SetWindowPos(pos__menu__detail__offset);

        // }

        ImVec2 menuWin_dim = ImGui::GetWindowSize();
        // ImGui::PushItemWidth(ImGui::GetWindowSize().x - _win_widthOffset - 110);
        ImGui::PushItemWidth(menuWin_dim.x - _win_widthOffset - 110);
        if(ImGui::InputText("##_title", &(toDetail->label), inpText_flags_label | ImGuiInputTextFlags_EnterReturnsTrue)) {
            toDetail->inChart->modified = true;
        }
        
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        if(ImGui::DateChooser(("##"+ptrToStr<gNC::gNODE*>(toDetail)+"_date").c_str(), toDetail->date)) {
            toDetail->inChart->modified = true;
        }

        ImGui::Separator();
        ImGui::PushItemWidth(menuWin_dim.x-_win_widthOffset);
        if(ImGui::InputText("##_desc", &(toDetail->desc), inpText_flags_desc | ImGuiInputTextFlags_EnterReturnsTrue)) {
            toDetail->inChart->modified = true;
        }
        
        ImGui::Separator();
        if(ImGui::InputTextMultiline("##_bodyText", &(toDetail->bodyText), ImVec2(menuWin_dim.x-_win_widthOffset, ImGui::GetTextLineHeight()*5), inpText_flags_bodyT | ImGuiInputTextFlags_EnterReturnsTrue)) {
            toDetail->inChart->modified = true;
        }

        ImGui::BeginGroup();
        if(ImGui::BeginChild("##window_node_links", ImVec2(-FLT_MIN, 0))) {
            if(ImGui::BeginTable("node_links", 3, node_links_table_flags, ImVec2(ImGui::GetWindowSize().x, 0))) {
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Link");
                ImGui::TableSetupColumn("Node");
                ImGui::TableHeadersRow();
                
                bool itemHovrd = false;
                for(int v=0; v<4; v++) {
                    std::vector<gNC::gLINK*>& vecRef = (v==0? toDetail->ln_in : (v==1? toDetail->ln_out : (v==2? toDetail->ln_add : toDetail->ln_share)));
                    ImGui::TableNextRow();
                    
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text((v==0? "in": (v==1? "out" : (v==2? "add" : "share"))));
                    // ImGui::TableSetColumnIndex(1);
                    ImGui::TableSetColumnIndex(1);
                    for(auto _link : vecRef) {
                        if(_link->draw__state==1) {
                            std::cout << "link state hovered" << std::endl;
                            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(66, 150, 250, 255));
                        }
                        else {
                            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(66, 150, 250, 102));
                        }
                        if(ImGui::Button((_link? ptrToStr<gNC::gLINK*>(_link).c_str() : "nullptr"))) {


                            linkPtr_menu__link_details = _link;
                            
                        }
                        // if(_link->draw__state==1) {
                            ImGui::PopStyleColor();
                        // }

                        if(ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                            itemHovrd = true;
                            _link->draw__state = 1;
                        }
                        // ImGui::Text((_link==nullptr? "nullptr" : ptrToStr<gNC::gLINK*>(_link).c_str()));
                    }
                    ImGui::TableSetColumnIndex(2);
                    for(auto _link : vecRef) {
                        gNC::gNODE* _printPtr = (v%2==0? _link->src : _link->dest);
                        if(_printPtr->draw__state==1) {
                            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(66, 150, 250, 255));
                        }
                        else {
                            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(66, 150, 250, 102));
                        }
                        if(ImGui::Button((_printPtr? ptrToStr<gNC::gNODE*>(_printPtr).c_str() : "nullptr"))) {
                            nodePtr_menu__node_details = _printPtr;
                            if(_printPtr) {
                                chart->setScreen_pos((dim__main[0]*0.5*(1.0/_DRAW_SCALAR.x)) - _printPtr->pos.x, (dim__main[1]*0.5*(1.0/_DRAW_SCALAR.y)) - _printPtr->pos.y);

                                ImGui::SetWindowFocus(_printPtr->addr.c_str());
                            
                            }
                        }
                        // if(_printPtr->draw__state==1) {
                            ImGui::PopStyleColor();
                        // }
                        if(ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                            itemHovrd = true;
                            _printPtr->draw__state = 1;
                            _link->draw__state = 1;
                            
                        }
                        // ImGui::Text((_printPtr==nullptr? "nullptr" : ptrToStr<gNC::gNODE*>(_printPtr).c_str()));
                    }
                }

                ImGui::EndTable();
            }
            ImGui::EndChild();
        }
        ImGui::EndGroup();
        
    }
    ImGui::End();

    if(init_node!=toDetail) init_node = toDetail;
}

void gNC::_menu__link_details(
    gNC::gLINK* toDetail,
    gNC::guiNodeChart *chart
) {
    assert(toDetail!=nullptr);
    static gNC::gLINK* init_link = nullptr;


    static ImGuiWindowFlags win_flags = 0;
    // win_flags |= ImGuiWindowFlags_NoResize;
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


    if(nodePtr_menu__node_details) {
        ImGui::SetNextWindowSizeConstraints(
            dim__menu__detail().min,
            ImVec2(dim__menu__detail().max.x, dim__menu__detail().min.y)
        );
        ImGui::SetNextWindowPos(
            ImVec2_add(pos__menu__detail__offset, ImVec2(0, dim__menu__detail().min.y)),
            0,
            ImVec2(0, 0)
        );
    }
    else {
        ImGui::SetNextWindowSizeConstraints(
            dim__menu__detail().min,
            dim__menu__detail().max
        );
        ImGui::SetNextWindowPos(
            pos__menu__detail__offset,
            0,
            ImVec2(0, 0)
        );
    }
    if(ImGui::Begin((" link: "+toDetail->addr).c_str(), NULL, win_flags)) {
        if(ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
            gNC::_DRAW_SCALAR_SCOLL_LOCK = true;
        }

        if(toDetail!=init_link) {
            /// Init called for a new link
            if(nodePtr_menu__node_details) {
                ImGui::SetWindowSize(dim__menu__detail().min);
            }
            else {
                ImGui::SetWindowSize(ImVec2(dim__menu__detail().min.x, dim__menu__detail().min.y));
            }
        }
        _winFocused__link_details = ImGui::IsWindowFocused();
        // ImGui::SetWindowPos(ImGui::GetWindowPos());
        // ImVec2 _winSize(0, 0);
        // if(nodePtr_menu__node_details) {
        //     ImGui::SetWindowPos(ImVec2(pos__menu__detail__offset.x, pos__menu__detail__offset.y+dim__menu__detail[1]*0.5));
        //     ImGui::SetWindowSize(ImVec2(dim__menu__detail[0], dim__menu__detail[1]*0.5));
        // }
        // else {
        //     ImGui::SetWindowPos(pos__menu__detail__offset);
        //     ImGui::SetWindowSize(dim__menu__detail);
        // }

        ImGui::PushItemWidth(ImGui::GetWindowSize().x - _win_widthOffset - 110);
        if(ImGui::InputText("##_title", &(toDetail->label), inpText_flags_label | ImGuiInputTextFlags_EnterReturnsTrue)) {
            toDetail->inChart->modified = true;
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        if(ImGui::DateChooser(("##"+ptrToStr<gNC::gLINK*>(toDetail)+"_date").c_str(), toDetail->date)) {
            toDetail->inChart->modified = true;
        }


        ImGui::Separator();
        ImGui::PushItemWidth(ImGui::GetWindowSize().x-_win_widthOffset);
        if(ImGui::InputText("##_desc", &(toDetail->desc), inpText_flags_desc | ImGuiInputTextFlags_EnterReturnsTrue)) {
            toDetail->inChart->modified = true;
        }

        ImGui::Separator();
        if(ImGui::InputTextMultiline("##_bodyText", &(toDetail->bodyText), ImVec2(ImGui::GetWindowSize().x-_win_widthOffset, ImGui::GetTextLineHeight()*5), inpText_flags_bodyT | ImGuiInputTextFlags_EnterReturnsTrue)) {
            toDetail->inChart->modified = true;
        }


        // ImGui::BeginGroup();
        // if(ImGui::BeginChild("##window_link_nodes", ImVec2(-FLT_MIN, 0))) {
        if(ImGui::BeginTable("link_nodes", 3, links_table_flags, ImVec2(ImGui::GetWindowSize().x-_win_widthOffset, 0))) {
            ImGui::TableSetupColumn("##_LCorner");
            ImGui::TableSetupColumn("Address");
            ImGui::TableSetupColumn("Type");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("src:");
            ImGui::TableSetColumnIndex(1);
            if(toDetail->src) {
                if(toDetail->src->draw__state==1) {
                    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(66, 150, 250, 255));
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(66, 150, 250, 102));
                }
            }
            if(ImGui::Button((toDetail->src? ptrToStr<gNC::gNODE*>(toDetail->src).c_str() : "nullptr"))) {
                nodePtr_menu__node_details = toDetail->src;
                if(toDetail->src) {
                    chart->setScreen_pos((dim__main[0]*0.5*(1.0/_DRAW_SCALAR.x)) - toDetail->src->pos.x, (dim__main[1]*0.5*(1.0/_DRAW_SCALAR.y)) - toDetail->src->pos.y);
                    ImGui::SetWindowFocus(toDetail->src->addr.c_str());

                }
            }
            if(toDetail->src) {
                if(ImGui::IsItemHovered()) {
                    toDetail->src->draw__state = 1;
                }
                ImGui::PopStyleColor();
            }
            // ImGui::Text((toDetail->src? ptrToStr<gNC::gNODE*>(toDetail->src).c_str() : "nullptr"));
            ImGui::TableSetColumnIndex(2);
            ImGui::Text((toDetail->type_src==1? "out" : "share"));
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("dest:");
            ImGui::TableSetColumnIndex(1);
            // ImGui::Text((toDetail->dest? ptrToStr<gNC::gNODE*>(toDetail->dest).c_str() : "nullptr"));
            if(toDetail->dest) {
                if(toDetail->dest->draw__state==1) {
                    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(66, 150, 250, 255));
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(66, 150, 250, 102));
                }
            }
            if(ImGui::Button((toDetail->dest? ptrToStr<gNC::gNODE*>(toDetail->dest).c_str() : "nullptr"))) {
                nodePtr_menu__node_details = toDetail->dest;
                if(toDetail->dest) {
                    chart->setScreen_pos((dim__main[0]*0.5*(1.0/_DRAW_SCALAR.x)) - toDetail->dest->pos.x, (dim__main[1]*0.5*(1.0/_DRAW_SCALAR.y)) - toDetail->dest->pos.y);
                    ImGui::SetWindowFocus(toDetail->dest->addr.c_str());
                }
            }
            if(toDetail->dest) {
                if(ImGui::IsItemHovered()) {
                    toDetail->dest->draw__state = 1;
                }
                ImGui::PopStyleColor();
            }
            ImGui::TableSetColumnIndex(2);
            ImGui::Text((toDetail->type_dest==0? "in" : "add"));


            ImGui::EndTable();
        }
        // ImGui::EndChild();
        // }
        // ImGui::EndGroup();
        if(ImGui::TreeNode("Link points")) {
            ImGui::SetCursorPosX(10);
            if(ImGui::BeginTable("link_points_raw", 3, links_table_flags, ImVec2(ImGui::GetWindowSize().x-_win_widthOffset-20, 0))) {
                ImGui::TableSetupColumn("Index");
                ImGui::TableSetupColumn("X");
                ImGui::TableSetupColumn("Y");
                ImGui::TableHeadersRow();
                
                for(size_t i=0; i<toDetail->link_points_raw.size(); i++) {
                    ImVec2 &currPoint = toDetail->link_points_raw[i];
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(formatNumber(i, 3, 0).c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushID(i);
                    if(ImGui::Selectable("", false, ImGuiSelectableFlags_SpanAllColumns)) {

                    }
                    ImGui::PopID();
                    if(ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                        if(project_draw_list) {
                            project_draw_list->AddCircle(ImVec2_multiply(ImVec2_add(currPoint, chart->screen_pos), _DRAW_SCALAR), 12, IM_COL32(200, 200, 10, 200), 10, 1);
                            // std::cout << "drew circle: " << formatContainer1(ImVec2_multiply(ImVec2_add(currPoint, chart->screen_pos), _DRAW_SCALAR), 2, 0, 0) << std::endl;
                        }
                    }
                    ImGui::SameLine();
                    ImGui::TextUnformatted(formatNumber(currPoint.x, 5, 0).c_str());
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted(formatNumber(currPoint.y, 5, 0).c_str());

                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }

    }
    ImGui::End();


    if(init_link!=toDetail) init_link = toDetail;
}


void gNC::_menu__rightClick(
    gNC::guiNodeChart* _chart
) {
    static bool init = false;

    static ImGuiIO& io = ImGui::GetIO();
    ImVec2 mousePos_temp = ImVec2_divide(io.MousePos, _DRAW_SCALAR);

    if(ImGui::BeginPopup("_menu__rightClick__default")) {

        ImGui::MenuItem("Default menu", NULL, false, false);
        if(ImGui::MenuItem("Create Node")) {
            _chart->NODE_create(
                mousePos_temp.x - _chart->screen_pos[0],
                mousePos_temp.y - _chart->screen_pos[1],
                "", "", ""
            );
            nodePtr_menu__node_details = _chart->lastAdded_NODE();
            // mouseAction_left = 0;
        }
            
        ImGui::EndPopup();
    }
    if(ImGui::BeginPopup("_menu__rightClick__node")) {
        assert(nodePtr_menu__rightClick);
        ImGui::MenuItem("Node menu", NULL, false, false);
        if(ImGui::MenuItem("Delete node")) {
            if(nodePtr_menu__node_details == nodePtr_menu__rightClick) nodePtr_menu__node_details = nullptr;
            _chart->NODE_delete(nodePtr_menu__rightClick);
        }
        ImGui::EndPopup();
    }
    if(ImGui::BeginPopup("_menu__rightClick__link")) {
        assert(linkPtr_menu__rightClick);
        ImGui::MenuItem("Link menu", NULL, false, false);
        if(ImGui::MenuItem("Delete link")) {
            if(linkPtr_menu__link_details == linkPtr_menu__rightClick) linkPtr_menu__link_details = nullptr;
            _chart->LINK_delete(linkPtr_menu__rightClick);
        }
        ImGui::EndPopup();
    }
    else {
        mouseAction_right = -1;
    }

    // if(ImGui::MenuItem("_menu__rightClick_"))

    if(!init) init=true;
}


void gNC::_menu__timeline(
    gNC::guiNodeChart* _chart
) {
    assert(_chart);

    static gNC::guiNodeChart* prev_chart = nullptr;
    static bool open    = true;
    static bool _init   = true;
    static bool _collapse   = true;
    static bool _collapse_prev  = true;
    /// The affected timeline container. 
    static std::vector<gNC::timeObject> *onTL = nullptr;

    /**
     * Variable to dictate whether an entire timeObject is being moved.
     * if == `nullptr` then no timeObject is being moved, otherwise the one with this gNC::gNode* is being moved
     */
    static gNC::gNODE* _moving_gNODE = nullptr;

    /// Variable to dictate a side is being moved. if == `(size_t)-1` then no side is being moved, 
    ///  otherwise a side is, and this holds the original value.
    static gNC::timeUnit    _moving_side__val = (size_t)-1;
    /// value for moving_side: channel
    static gNC::timeUnit    _moving_side__channel = 0;
    /// Value for moving_side: gNODE. If this is `nullptr` then move all/both timeObject side(s) that exist at `_moving_side__val`, otherwise move the
    ///  timeObject containing this gNODE* only (assuming no conflicts at new value)
    static gNC::gNODE*      _moving_side__gNODE = nullptr;
    static std::vector<gNC::gNODE*> _moving_side__gNODE_S;
    /// Integer id vlaue for what side/edge of the timeObject is being moved, with: `1`-start side; `2`-end side
    static int              _moving_side__side  = 0;

    /// The padding value for the detection region for the mouse that separates moving the timeObject to moving the side.
    /// This also specifis the mininum width a timeObject can have because for "semantics sake" a timeObject cannot be smaller than 6x of these units wide.
    static int __moving_side_detect_padding = 5;
    /// Minimum distance between timeObject's at which they'll snap together.
    static gNC::timeUnit    _timeUnit_snapDistance = 15;


    if(_chart != prev_chart || __GLOBAL_FLAGS__WIN_RESIZED>0) _init = true;

    _collapse_prev = _collapse;
    if(!open) {
        _collapse = true;
    }
    else {}

    open = true;
    static ImGuiWindowFlags win_flags = ImGuiWindowFlags_NoMove;
    
    ImGuiIO &io = ImGui::GetIO();

    

    ImGui::SetNextWindowSizeConstraints(ImVec2(dim__win_timeline().x, dim__win_timeline().y*0.01), ImVec2(dim__win_timeline().x, dim__main.y));
    ImGui::SetNextWindowPos(
        ImVec2(
            0, //dim__main.x,// - ImGui::GetWindowSize().x,
            dim__main.y// - ImGui::GetWindowSize().y
        ),
        0,
        ImVec2(0, 1)
    );
    ImGui::Begin("Timeline", &open, win_flags);
    if(_init) ImGui::SetWindowSize(dim__win_timeline());
    if(__PROGRAM_FRAMES<=1) {
        ImGui::SetWindowCollapsed(true);
        ImGui::SetWindowPos(ImVec2(
            dim__main.x - ImGui::GetWindowSize().x,
            dim__main.y - ImGui::GetWindowSize().y
        ));
    }
    // ImGui::SetWindowCollapsed(_collapse);
    _collapse = ImGui::IsWindowCollapsed();

    // ImGuiIO &io = ImGui::GetIO();
    ImDrawList* timeline_drawList = ImGui::GetWindowDrawList();

    if(_collapse != _collapse_prev || __GLOBAL_FLAGS__WIN_RESIZED>0) {
        if(_collapse_prev) _init = true;
        //this iteration had a state change for _collapse
        ImGui::SetWindowPos(ImVec2(
            dim__main.x - ImGui::GetWindowSize().x,
            dim__main.y - ImGui::GetWindowSize().y
        ));
    }

    /// (relative to window) timeline child window position
    static ImVec2 timeline_pos      = ImVec2(50, 50);
    /// flags for timeline child window
    static ImGuiChildFlags timeline_flags = ImGuiChildFlags_Border;
    /// scroll value of the timeline child window
    static ImVec2 scrollVal(0, 0);

    /// custom xy scalar values for timeObject dimension
    static ImVec2 gui_objDim__scal(1, 1);
    /// timeObject default height in pixels
    static float gui_objDim__height = 50;

    gNC::timeline &TL_ref = _chart->TimeLine;


    ImVec2 timeline_dim = ImVec2(dim__win_timeline().x, TL_ref.get_numChannels_used()*gui_objDim__height*gui_objDim__scal.y);


    if((gNC::_winHover__timeline = inRegion(io.MousePos, ImGui::GetWindowPos(), ImVec2_add(ImGui::GetWindowPos(), ImGui::GetWindowSize())))) {
        gNC::_DRAW_SCALAR_SCOLL_LOCK = true;
    }
    if(!_collapse) {
        // if((gNC::_winHover__timeline = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))) {
        //     gNC::_DRAW_SCALAR_SCOLL_LOCK = true;
            
        // }
        // if(_init) {
        TL_ref.update_forVisuals();
        // }

        scrollVal.x = ImGui::GetScrollX();
        scrollVal.y = ImGui::GetScrollY();
        ImVec2 _relWinPos = ImGui::GetWindowPos();
        ImGui::SetCursorPos(timeline_pos);
        // ImGui::SetNextWindowPos(timeline_pos);
        ImGui::BeginChild("timeline", timeline_dim, timeline_flags);
        
        /// offset value between the timeline child window's absolute position and the scroll values
        /// ImVec2(_relWinPos.x - scrollVal.x, _relWinPos.y - scrollVal.y);
        static ImVec2 placeOffs = ImVec2_subtract(_relWinPos, scrollVal);
        placeOffs = ImVec2_subtract(_relWinPos, scrollVal);


        /**
         * Special absolute "position" values for the absolute xy coordinates based on timeObject values where:
         *  - `x` : x coord = timeUnit value in the timeline
         *  - `y` : y coord = channel for the timeObject.
         */
        static ImVec2 timeObject_pos(0, 0);
        /// XY pixel dimension of the timeObject (y value is not specific to any timeObject and is universal)
        static ImVec2 timeObject_dim(0, gui_objDim__height*gui_objDim__scal.y);

        /// @brief xy offset values from io.MousePos - timeObject_pos
        static ImVec2 timeObject_cursorOffs(0, 0);

        static ImVec2 mousePos_insideTimeline(0, 0); // MousePos coordinate in relative terms of the timeline: [x=x, y=y]
        static ImVec2 mousePos_relativeValues(0, 0); // MousePos coordinate in relative terms of the timeline: [x=timeUnit, y=channel]

        // std::cout << std::endl;
        // std::cout << mousePos_relativeValues.x << " "<< mousePos_relativeValues.y << "  : ";
        // std::cout << ((_moving_side__val.value!=(size_t)-1)? _moving_side__val.value : -1) << "  ";
        // std::cout << formatNumber((_moving_side__val.value!=(size_t)-1), 5, 0, "left") << " ";

        static auto _updateRelativeMouse = [](ImVec2 mousePos) {
            mousePos_insideTimeline = ImVec2_subtract(ImVec2_subtract(mousePos, timeline_pos), placeOffs);
            mousePos_relativeValues = ImVec2(
                mousePos_insideTimeline.x,
                floor(mousePos_insideTimeline.y / timeObject_dim.y) + 1
            );
            if(mousePos_relativeValues.y < 1) mousePos_relativeValues.y = 1;
            if(mousePos_relativeValues.x < 0) mousePos_relativeValues.x = 0;
            return mousePos_relativeValues;
        };

        static auto _timeUnit_to_x = [](timeUnit _timeUnit) {
            return _timeUnit.value;// + timeObject_cursorOffs.x;
        };
        static auto _channel_to_y = [](size_t _channel, float _channelHeight) {
            return (_channel-1) * _channelHeight + placeOffs.y + timeline_pos.y;
        };

        /**
         * Special relative "position" values for the timeObject values based on absolute xy coordinates:
         *  - `x` : timeUnit value in the timeline = x
         *  - `y` : channel for the timeObject. = y
         * 
         * Tl:dr: position of the cursor in terms of the timeline
         */
        static ImVec2 _relativePos(0, 0);
        static auto _update = [] (ImVec2 mousePos, timeObject& obj) {
            _updateRelativeMouse(mousePos);
            timeObject_dim.x = (obj.end - obj.start).value;
            timeObject_pos = ImVec2(
                timeline_pos.x + placeOffs.x + obj.start.value,
                timeline_pos.y + placeOffs.y + timeObject_dim.y*(obj.channel-1)
            );
            _relativePos.x = mousePos_insideTimeline.x - timeObject_cursorOffs.x;
            _relativePos.y = floor(mousePos_insideTimeline.y / timeObject_dim.y) + 1;
            if(_relativePos.y < 1) _relativePos.y = 1;
            if(_relativePos.x < 0) _relativePos.x = 0;
            return _relativePos;
        };
        
        
        static std::vector<ImU32> timeObject_colour_border{ IM_COL32(230, 230, 230, 230)};
        static std::vector<ImU32> timeObject_colour_bg{     IM_COL32(184, 184, 184, 184), IM_COL32(214, 214, 214, 214), IM_COL32(244, 244, 244, 10)};
        static size_t tO_colBord    = 0;
        static size_t tO_colBG      = 0; // {`0`-default, `1`-hovered, `2`-selected}

        bool cosmetics__cursorChanged = false;

        
        
        // TL_ref.verbose_exceptions = true;
        if(_moving_gNODE) {
            // ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            timeObject obj = TL_ref.get_timeObject(_moving_gNODE);
            _update(io.MousePos, obj);

            ImVec2 draw_obj = ImVec2_subtract(io.MousePos, timeObject_cursorOffs);


            if(_SETTINGS.get("Timeline").get("Use timeObject Snap")) {
                /// The "border" timeUnit value where if currently moving timeObject's side reaches another timeObjects side, it'll snap moving timeObject to that side.
                gNC::timeUnit snapCheck[2] = {
                    _relativePos.x, // - (_timeUnit_snapDistance.value > _relativePos.x? _timeUnit_snapDistance.value - _relativePos.x : _timeUnit_snapDistance.value),
                    _relativePos.x + timeObject_dim.x
                };
                snapCheck[0] -= (_timeUnit_snapDistance.value > _relativePos.x? _timeUnit_snapDistance.value - _relativePos.x : _timeUnit_snapDistance.value);
                snapCheck[1] += (_timeUnit_snapDistance.value);

                std::vector<gNC::gNODE*> _snapCollid = TL_ref.get_timeObjects_inTimeUnit(snapCheck[0], _relativePos.y, onTL);  //get_sides(snapCheck[0], _relativePos.y, 0);
                for(size_t i=0; i<_snapCollid.size(); i++) { // erase the moving node from collision checks
                    if(_snapCollid.at(i)==_moving_gNODE) {
                        auto itr = _snapCollid.begin();
                        std::advance(itr, i);
                        _snapCollid.erase(itr);
                        break;
                    }
                }
                if(_snapCollid.size()!=0) {
                    snapCheck[0] = TL_ref.get_timeObject(_snapCollid[0], onTL).end;
                    _relativePos.x = snapCheck[0].value;
                }
                else {
                    _snapCollid = TL_ref.get_timeObjects_inTimeUnit(snapCheck[1], _relativePos.y, onTL);
                    for(size_t i=0; i<_snapCollid.size(); i++) { // erase the moving node from collision checks
                        if(_snapCollid.at(i)==_moving_gNODE) {
                            auto itr = _snapCollid.begin();
                            std::advance(itr, i);
                            _snapCollid.erase(itr);
                            break;
                        }
                    }
                    if(_snapCollid.size()!=0) {
                        snapCheck[1] = TL_ref.get_timeObject(_snapCollid[0], onTL).start;
                        _relativePos.x = snapCheck[1].value-timeObject_dim.x;
                    }
                }


                if(_SETTINGS.get("View").get("Draw timeObject sides")) {
                    timeline_drawList->AddLine(
                        ImVec2(snapCheck[0].value + timeline_pos.x + placeOffs.x, _channel_to_y(_relativePos.y, timeObject_dim.y)),
                        ImVec2(snapCheck[0].value + timeline_pos.x + placeOffs.x, _channel_to_y(_relativePos.y, timeObject_dim.y) + timeObject_dim.y),
                        IM_COL32(10, 200, 10, 200)
                    );
                    timeline_drawList->AddLine(
                        ImVec2(snapCheck[1].value + timeline_pos.x + placeOffs.x, _channel_to_y(_relativePos.y, timeObject_dim.y)),
                        ImVec2(snapCheck[1].value + timeline_pos.x + placeOffs.x, _channel_to_y(_relativePos.y, timeObject_dim.y) + timeObject_dim.y),
                        IM_COL32(10, 200, 10, 200)
                    );
                }
            }

            if(TL_ref.add_timeObject(_moving_gNODE, _relativePos.x, _relativePos.x+timeObject_dim.x, _relativePos.y, 0, onTL)) {
                int moveCode = TL_ref.move_timeObject(_moving_gNODE, _relativePos.x, _relativePos.x+timeObject_dim.x, _relativePos.y, 0, onTL);
            }

            timeline_drawList->AddRectFilled(draw_obj, ImVec2_add(draw_obj, timeObject_dim), timeObject_colour_bg[0], 2, 0);
            timeline_drawList->AddRect(      draw_obj, ImVec2_add(draw_obj, timeObject_dim), timeObject_colour_border[0], 2, 0, 1);

            // if(!guiKeys.isHolding(ImGuiKey_MouseLeft)) {
            if(!keyBinds.pressing("MouseLeft")) {
                /// This iteration is a state change where the mouseLeft went from holding -> not holding

                if(TL_ref.add_timeObject(_moving_gNODE, _relativePos.x, _relativePos.x+timeObject_dim.x, _relativePos.y, 0, nullptr)) {
                    int moveCode = TL_ref.move_timeObject(_moving_gNODE, _relativePos.x, _relativePos.x+timeObject_dim.x, _relativePos.y, 0, nullptr);
                }
            }
        }
        else if(_moving_side__val.value != (size_t)-1) {
            // ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            if(_moving_side__gNODE) {
                switch (_moving_side__side) {
                    case 0: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll); break;
                    case 1:
                    case 2: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW); break;
                    case 3: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW); break;
                    default:
                        break;
                }

            }

            cosmetics__cursorChanged = true;
            _updateRelativeMouse(io.MousePos);
            timeUnit _newSideVal = mousePos_relativeValues.x;

            if(_SETTINGS.get("Timeline").get("Use timeObject Snap")) {
                gNC::timeUnit snapCheck = _newSideVal;
                switch (_moving_side__side) {
                case 0: break;
                case 1:
                    snapCheck -= (_timeUnit_snapDistance > _newSideVal? _timeUnit_snapDistance - _newSideVal : _timeUnit_snapDistance);
                    break;
                case 2:
                    snapCheck += (_timeUnit_snapDistance);
                    break;
                default:
                    std::cout << "ERROR: a moving side's side id is somehow nether 1 or 2."<<std::endl;
                    break;
                }
            
                std::vector<gNC::gNODE*> _snapCollid = TL_ref.get_timeObjects_inTimeUnit(snapCheck, _moving_side__channel, onTL);

                for(auto _gnode : _moving_side__gNODE_S) {
                    for(size_t i=0; i<_snapCollid.size(); i++) {
                        if(_snapCollid.at(i)==_gnode) {
                            auto itr = _snapCollid.begin();
                            std::advance(itr, i);
                            _snapCollid.erase(itr);
                            break;
                        }
                    }
                }
                if(_snapCollid.size()!=0) {
                    gNC::timeObject collidObj = TL_ref.get_timeObject(_snapCollid[0], onTL);
                    snapCheck = (_moving_side__side==1? collidObj.end : collidObj.start);
                    _newSideVal = snapCheck;
                }

                if(_SETTINGS.get("View").get("Draw timeObject sides"))  {
                    timeline_drawList->AddLine(
                        ImVec2(snapCheck.value + timeline_pos.x + placeOffs.x, _channel_to_y(_moving_side__channel, timeObject_dim.y)),
                        ImVec2(snapCheck.value + timeline_pos.x + placeOffs.x, _channel_to_y(_moving_side__channel, timeObject_dim.y) + timeObject_dim.y),
                        IM_COL32(10, 200, 10, 200)
                    );
                }
            }

            int call_code = 0;
            if((call_code = TL_ref.move_sides(_moving_side__val, _newSideVal, _moving_side__gNODE, _moving_side__channel, __moving_side_detect_padding, onTL))) {
                std::cout << "forVisuals move_sides error: " << call_code << std::endl;
            }

            // if(!guiKeys.isHolding(ImGuiKey_MouseLeft)) {
            if(!keyBinds.pressing("MouseLeft")) {
                /// A side moving operation is to take effect on the "real" timeline instead of the "forVisuals purpose" one.

                call_code = 0;
                if((call_code = TL_ref.move_sides(_moving_side__val, _newSideVal, _moving_side__gNODE, _moving_side__channel, __moving_side_detect_padding, nullptr))) {
                    std::cout << "move_sides error: " << call_code << std::endl;
                }
            }
        }
        else {
            // ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        }

        /// temporary boolean for whether left mouse button was just recently registered as holding
        bool recentlyHolding_mouseLeft = keyBinds.clicked("MouseLeft");

        _updateRelativeMouse(io.MousePos);


        // std::cout << " | " << formatVector(_affctd, 0, 0) << " ";
        if(!_moving_gNODE && _moving_side__val.value==(size_t)-1) {
            std::vector<gNC::gNODE*> _affctd = TL_ref.get_sides(mousePos_relativeValues.x, mousePos_relativeValues.y, __moving_side_detect_padding, nullptr);
            if(_affctd.size()>0) {
                // std::cout << "_affctd:" << formatVector(_affctd, 0, 0) << " ";
                cosmetics__cursorChanged = true;
                if(_affctd.size()==1) _moving_side__side = TL_ref.get_timeObject(_affctd[0]).is_side(mousePos_relativeValues.x, __moving_side_detect_padding, true);
                else _moving_side__side = 0;
                // std::cout << _moving_side__side << std::endl;
                // if(!_moving_gNODE && _moving_side__val.value == (size_t)-1) {
                    if(recentlyHolding_mouseLeft) {
                        _moving_side__channel   = mousePos_relativeValues.y;
                        _moving_side__val       = mousePos_relativeValues.x;
                        _moving_side__gNODE_S = _affctd;
                        if(_affctd.size()==1) {
                            _moving_side__gNODE = _affctd[0];
                            // _moving_side__side = TL_ref.get_timeObject(_affctd[0]).is_within(_moving_side__val);
                        
                        }
                        else {
                            // _moving_side__side = 0;

                        }
                    }
                // }

                switch (_moving_side__side) {
                    case 0: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll); break;
                    case 1:
                    case 2: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW); break;
                    // case 3: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW); break;
                    default:
                        std::cout << "ERROR: _moving_side__side is an invalid value: `!=(0,1,2)`" << std::endl;
                        break;
                }
            }
            else {

            }
        }
        // std::cout << " msGNODE:"<< (_moving_side__gNODE? ptrToStr(_moving_side__gNODE) : "nullptr") << " ";

        for(size_t i=0; i<TL_ref.size(); i++) {
            tO_colBG = 0;
            tO_colBord = 0;

            timeObject &obj = TL_ref.forVisuals.at(i);
            _update(io.MousePos, obj);

            if(_moving_side__val.value==(size_t)-1 && inRegion(io.MousePos, timeObject_pos, ImVec2_add(timeObject_pos, timeObject_dim))) {
                if(recentlyHolding_mouseLeft) {
                    /// if mouseLeft is clicked in this program frame

                    if(!_moving_gNODE) {
                        /// moving the node object
                        _moving_gNODE = obj.objNode;
                        timeObject_cursorOffs = ImVec2_subtract(io.MousePos, timeObject_pos);
                    }
                }

            }
            else {}

            // object-interaction colour stuff
            if(inRegion(io.MousePos, ImVec2(timeObject_pos.x-__moving_side_detect_padding, timeObject_pos.y), ImVec2(timeObject_pos.x+timeObject_dim.x+__moving_side_detect_padding, timeObject_pos.y+timeObject_dim.y))) {
                /// within region of timeObject with __moving_side_detect_padding

                if(recentlyHolding_mouseLeft) {
                    // tO_colBG = 2;
                }
                else {
                    tO_colBG = 1;
                    if(!cosmetics__cursorChanged) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
                }
            }
            else {
                tO_colBG = 0;
                if(!cosmetics__cursorChanged) ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            }
            
            if(_moving_gNODE==obj.objNode) {
                tO_colBG = 2;
            }

            /// Draw the timeObject
            {
                timeline_drawList->AddRectFilled(timeObject_pos, ImVec2_add(timeObject_pos, timeObject_dim), timeObject_colour_bg[tO_colBG], 2, 0);
                timeline_drawList->AddRect(timeObject_pos, ImVec2_add(timeObject_pos, timeObject_dim), timeObject_colour_border[tO_colBord], 2, 0, 1);
                timeline_drawList->AddText(ImVec2_add(timeObject_pos, ImVec2_multiply(timeObject_dim, ImVec2(0, 0.5))), IM_COL32(10, 10, 10, 255), ptrToStr<gNC::gNODE*>(obj.objNode).c_str());
                
                if(_SETTINGS[1][1]) {
                    timeline_drawList->AddLine(
                        ImVec2_subtract(timeObject_pos, ImVec2(__moving_side_detect_padding, 0)),
                        ImVec2_subtract(timeObject_pos, ImVec2(__moving_side_detect_padding, -timeObject_dim.y)),
                        IM_COL32(250, 10, 10, 250)
                    );
                    timeline_drawList->AddLine(
                        ImVec2_add(timeObject_pos, ImVec2(__moving_side_detect_padding, 0)),
                        ImVec2_add(timeObject_pos, ImVec2(__moving_side_detect_padding, timeObject_dim.y)),
                        IM_COL32(250, 10, 10, 250)
                    );
                    timeline_drawList->AddLine(
                        ImVec2_add(timeObject_pos, ImVec2(__moving_side_detect_padding*2, 0)),
                        ImVec2_add(timeObject_pos, ImVec2(__moving_side_detect_padding*2, timeObject_dim.y)),
                        IM_COL32(10, 10, 250, 250)
                    );


                    timeline_drawList->AddLine(
                        ImVec2(timeObject_pos.x + timeObject_dim.x + __moving_side_detect_padding, timeObject_pos.y),
                        ImVec2(timeObject_pos.x + timeObject_dim.x + __moving_side_detect_padding, timeObject_pos.y+timeObject_dim.y),
                        IM_COL32(250, 10, 10, 250)
                    );
                    timeline_drawList->AddLine(
                        ImVec2(timeObject_pos.x + timeObject_dim.x - __moving_side_detect_padding, timeObject_pos.y),
                        ImVec2(timeObject_pos.x + timeObject_dim.x - __moving_side_detect_padding, timeObject_pos.y+timeObject_dim.y),
                        IM_COL32(250, 10, 10, 250)
                    );
                    timeline_drawList->AddLine(
                        ImVec2_subtract(ImVec2(timeObject_pos.x+timeObject_dim.x, timeObject_pos.y), ImVec2(__moving_side_detect_padding*2, 0)),
                        ImVec2_subtract(ImVec2(timeObject_pos.x+timeObject_dim.x, timeObject_pos.y), ImVec2(__moving_side_detect_padding*2, -timeObject_dim.y)),
                        IM_COL32(10, 10, 250, 250)
                    );
                }
            }
        }

        if(inRegion(
            io.MousePos,
            ImVec2_add(timeline_pos, placeOffs),
            ImVec2_add(ImVec2_add(timeline_pos, placeOffs), timeline_dim)
        )) {
            // circleCol = IM_COL32(50, 255, 50, 255);
            // if(guiKeys.isHolding(ImGuiKey_MouseLeft)) circleCol = IM_COL32(50, 50, 255, 255);
        }
        else {
            // circleCol = IM_COL32(255, 50, 50, 255);
        }
        // timeline_drawList->AddCircle(io.MousePos, 10, circleCol, 10, 2);

        ImGui::EndChild();
    }
    else {

    }
    ImGui::End();

    // std::cout << std::endl;


    // if(guiKeys.isHolding(ImGuiKey_MouseLeft)) {
    if(keyBinds.pressing("MouseLeft")) {
        onTL = &TL_ref.forVisuals;
    }
    else {
        if(_moving_gNODE) {
            /// This iteration is a state change where it went from holding to not.

        }
        
        onTL = nullptr;
        _moving_gNODE = nullptr;
        _moving_side__val = (size_t)-1;
        _moving_side__gNODE = nullptr;
        _moving_side__gNODE_S.clear();
        _moving_side__side  = 0;
        
        // std::cout << "local pointers \"release\"" << std::endl;
    }
    _init = false;
    prev_chart = _chart;
}


std::vector<std::string> __parsePath(std::string __path, char sep_symb) {
    assert(__path.size()>0);
    // std::cout << "__parsePath: " << __path << std::endl;
#if _WIN32
#else
    assert(__path[0]==sep_symb);
#endif
    std::vector<std::string> _parsed;
    for(int i=0; i<__path.length(); i++) {
        // // std::cout<<__path.at(i); std::cout.flush();
        if(__path.at(i)==sep_symb && i<__path.length()-1) {
            _parsed.push_back("");
            continue;
        }
        else if(__path.at(i)==sep_symb) break;
        _parsed[_parsed.size()-1] += __path.at(i);
    }
    // std::cout << std::endl;
    return _parsed;
}

int __getPWD_fileCont(
    std::string _path,
    std::vector<fileCont>& _retur
) {
    std::vector<fileCont> _temp;
    DIR* dir;
    dirent* ent;
    if((dir=opendir(_path.c_str()))!=NULL) {
        while((ent=readdir(dir))!=NULL) {
            struct stat _tempStat;
            if(stat((_path+ent->d_name).c_str(), &_tempStat)==-1) {
                return -1;
            }
            _temp.push_back(fileCont{
                std::string(ent->d_name),
                ent->d_ino,
                ent->d_type,
                _tempStat.st_size,
                _tempStat.st_atime,
                _tempStat.st_mtime,
                _tempStat.st_ctime
            });
        }
        closedir(dir);
    }
    else return -1;

    _retur = _temp;

    return 0;
}


std::string                 gNC::_file__fileExplorer = "";
std::vector<std::string>    gNC::_mode__types{"NULL", "Open", "Save", "Load", "Save As.."};
int                         gNC::_mode__fileExplorer = 0;
int                         gNC::_mode__fileExplorer_prev = 0;
std::vector<std::string>    gNC::_valid__extensions;

void gNC::_menu__fileExplorer() {
    static std::string currDir = __dir_saveFiles;
    static std::string dir_history;
    
    // static std::vector<dirent*>     _pwdCont;
    // static std::vector<struct stat> _pwdCont_stat;
    /**
     * `std::vector` of all the `fileCont` structs for each dir/file in given
     * directory.
     */
    static std::vector<fileCont>    _pwdFileCont;

    static int selected = -1;

    static int scr_width = 0;
    static int scr_height= 0;

    static bool win_open = true;

    bool isInit = false;

    static bool newDir  = true;
    static std::string newDirStr = "";

    bool valid_selection = false;

    if(!_mode__fileExplorer) {
        _mode__fileExplorer_prev = _mode__fileExplorer;
        return;
    }
    // std::cout<<"c1"<<std::endl;
    if(!_mode__fileExplorer_prev) {
        isInit = true;
        //This is the "first"/init iteration of the new fileExplorer call.
        //This is the init call
        scr_width   = dim__win_fileExplorer().x;
        scr_height  = dim__win_fileExplorer().y;
        currDir     = __dir_saveFiles;
        selected    = -1;
        win_open    = true;
        _file__fileExplorer = "";
    }
    // std::cout<<"currDir:"<<currDir << std::endl;
    if(__getPWD_fileCont(currDir, _pwdFileCont)) {
        perror("__getPWD_fileCont failed: ");
        exit(1);
    }
    // if(__getPWD_content(currDir, _pwdCont)) { //error occured
    // }
    // if(__getPWD_stats(currDir, _pwdCont, _pwdCont_stat)) { //error occured
    // }

    /**
     * @brief container for the selected items/(files/folders) in current directory
     * @note needs to be updated whenever directory is changed
     */
    static std::vector<bool> table_selections(_pwdFileCont.size(), false);
    /**
     * @brief string that's stored/passed to inputtext fields for the selected files
     * @note needs to be updated whenever directory is changed
     */
    static std::string enterBarFileStr = "";
    static bool mode2_inpCorrect = false; //whether the input text in mode 2 is valid (to prevent it from disappearing)

    static std::vector<std::string> _parsed_path = __parsePath(currDir);

    if(!_mode__fileExplorer_prev || newDir) {
        table_selections = std::vector<bool>(_pwdFileCont.size(), false);
        enterBarFileStr = "";
        _parsed_path = __parsePath(currDir);

        mode2_inpCorrect = false;
        newDir = false;
    }


    _mode__fileExplorer_prev = _mode__fileExplorer;

    static ImGuiWindowFlags win_flags = 0;
    win_flags |= ImGuiWindowFlags_NoResize;
    win_flags |= ImGuiWindowFlags_NoScrollbar;
    win_flags |= ImGuiWindowFlags_NoCollapse;
    win_flags |= ImGuiWindowFlags_NoScrollbar;
    win_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    static ImGuiChildFlags child_flags = 0;
    child_flags |= ImGuiChildFlags_Border;

    static ImGuiTableFlags table_flags_headings = 0;
    static ImGuiTableFlags table_flags_contents = 0;
    table_flags_contents |=
        ImGuiTableFlags_NoBordersInBody |
        ImGuiTableFlags_Reorderable |
        ImGuiTableFlags_Hideable |
        ImGuiTableFlags_Sortable |
        ImGuiTableFlags_SortMulti |
        ImGuiTableFlags_SizingStretchProp |
        ImGuiTableFlags_ScrollY;


    if(ImGui::Begin("File Explorer", &win_open, win_flags)) {
        if(isInit) {
            //window init stuff
            ImGui::SetWindowSize(dim__win_fileExplorer());
        }
        if(!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
            ImGui::SetWindowFocus("File Explorer");
        }


        ImGui::SetCursorPos(_pos__sub_fileExplorer_addressBar()[0]);
        if(ImGui::BeginChild("address_bar", _pos__sub_fileExplorer_addressBar(false)[2], child_flags)) {
            ImVec2 strPath_pos(padding__content_fileExplorer.x, _pos__sub_fileExplorer_addressBar(false)[2].y*0.5);
            ImGui::SetCursorPos(strPath_pos);
            ImGui::TextUnformatted("/");
            strPath_pos.x += ImGui::GetFontSize()-5;
            for(int i=0; i<_parsed_path.size(); i++) {
                ImGui::PushID(i);
                ImGui::SetCursorPos(strPath_pos);
                ImGui::SetNextItemWidth(_parsed_path[i].length());
                if(ImGui::Button(_parsed_path[i].c_str(), ImVec2(_parsed_path[i].length()*(ImGui::GetFontSize()-6)+7, 0))) {
                    // std::cout << "newDir selected" << std::endl;
                    std::string _tempDir = "/";
                    for(int ii=0; ii<=i; ii++) _tempDir += _parsed_path[ii] + "/"; 
                    newDirStr = _tempDir;
                    newDir = true;
                    // selected = -1;
                    // std::cout << "newDir: " << newDirStr<<std::endl;
                }
                strPath_pos.x += _parsed_path[i].length()*(ImGui::GetFontSize()-6)+7;
                ImGui::PopID();
                ImGui::SetCursorPos(strPath_pos);
                ImGui::TextUnformatted("/");
                strPath_pos.x +=ImGui::GetFontSize()-5;
            }
            if(isInit) ImGui::SetScrollX(strPath_pos.x);
        }
        ImGui::EndChild();
        ImGui::SetCursorPos(_pos__sub_fileExplorer_searchBar()[0]);
        if(ImGui::BeginChild("search_bar", _pos__sub_fileExplorer_searchBar(false)[2], child_flags)) {

        }
        ImGui::EndChild();
        ImGui::SetCursorPos(_pos__sub_fileExplorer_contents()[0]);
        if(ImGui::BeginChild("contents", _pos__sub_fileExplorer_contents(false)[2], child_flags, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
            // ImGui::SetWindowFocus("contents");
            if(isInit) ImGui::SetKeyboardFocusHere();
            ImGui::SetCursorPos(ImVec2(0, 0));
            if(ImGui::BeginTable("_currDir", 4, table_flags_contents, ImVec2(_pos__sub_fileExplorer_contents(false)[2].x-2, _pos__sub_fileExplorer_contents(false)[2].y-2))) {
                
                ImGui::SetCursorPosX(padding__content_fileExplorer.x);

                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, _pos__sub_fileExplorer_contents(false)[2].x*0.45);
                ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, _pos__sub_fileExplorer_contents(false)[2].x*0.12);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, _pos__sub_fileExplorer_contents(false)[2].x*0.08);
                ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, _pos__sub_fileExplorer_contents(false)[2].x*0.35);
                
                ImGui::TableSetupScrollFreeze(0, 1);
                // ImGui::TableHeadersRow();
                ImGui::TableNextRow(ImGuiTableRowFlags_Headers, ImGui::GetTextLineHeight()+padding__content_fileExplorer.y);
                for(int i=0; i<4; i++) {
                    if(!ImGui::TableSetColumnIndex(i)) continue;
                    if(i==0) ImGui::SetCursorPosX(padding__content_fileExplorer.x);
                    // ImGui::SetCursorPosY(padding__content_fileExplorer.y);
                    ImGui::TableHeader(ImGui::TableGetColumnName(i));
                }

                for(int i=0; i<_pwdFileCont.size(); i++) {
                    const bool item_is_selected = table_selections[i];

                    ImGui::PushID(i);
                    ImGui::TableNextRow();
                    if(ImGui::TableSetColumnIndex(0)) {
                        // ImGui::TextUnformatted(_pwdFileCont[i].name.c_str());
                        ImGui::SetCursorPosX(padding__content_fileExplorer.x);
                        ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;
                        if(ImGui::Selectable(_pwdFileCont[i].name.c_str(), item_is_selected, selectable_flags, ImVec2(0, 0))) {
                            if(ImGui::GetIO().KeyCtrl) {
                                table_selections[i] = !item_is_selected;
                            }
                            else {
                                for(size_t ii=0; ii<table_selections.size(); ii++) table_selections[ii] = false;
                                table_selections[i] = true;
                            }
                            if(guiKeys.keyPeriod(655)>0) {
                                //check if a file/folder has been double-clicked
                                // std::cout << "LMB double clicked" << std::endl;
                                switch (_pwdFileCont[i].type) {
                                    case DT_REG:
                                        if(validExtension(_pwdFileCont[i].name.c_str(), _valid__extensions)) {
                                            _file__fileExplorer = currDir + _pwdFileCont[i].name;
                                            win_open = false;
                                            // std::cout << " newDir: " << _file__fileExplorer << std::endl;

                                        }
                                        break;
                                    case DT_DIR:
                                        if(_pwdFileCont[i].name==".") {}
                                        else if(_pwdFileCont[i].name=="..") {
                                            std::string _tempDir = "/";
                                            for(int ii=0; ii<_parsed_path.size()-1; ii++) _tempDir += _parsed_path[ii] + "/";
                                            newDirStr = _tempDir;
                                            newDir = true;
                                        }
                                        else {
                                            newDirStr = currDir + _pwdFileCont[i].name + "/";
                                            newDir = true;
                                        }

                                        break;
                                }
                            }
                        }
                    }
                    if(ImGui::TableSetColumnIndex(1)) {
                        ImGui::TextUnformatted(formatBytes(_pwdFileCont[i].size, 6, 2, false, "right").c_str());
                    }
                    if(ImGui::TableSetColumnIndex(2)) {
                        // ImGui::TextUnformatted((_pwdFileCont[i].type==DT_REG? ))
                        std::string _type = "";
                        switch (_pwdFileCont[i].type) {
                            case DT_REG: _type="file";  break;
                            case DT_DIR: _type="folder";break;
                        }
                        ImGui::TextUnformatted(_type.c_str());
                    }
                    if(ImGui::TableSetColumnIndex(3)) {
                        // ImGui::TextUnformatted(formatNumber<time_t>(_pwdFileCont[i].mtime, 0, 0).c_str());
                        ImGui::TextUnformatted(ctime(&_pwdFileCont[i].mtime));
                    }

                    ImGui::PopID();
                }
                ImGui::EndTable();
            }

        }
        ImGui::EndChild();
        ImGui::SetCursorPos(_pos__sub_fileExplorer_detailsPanel()[0]);
        if(ImGui::BeginChild("details_panel", _pos__sub_fileExplorer_detailsPanel(false)[2], child_flags)) {
            // std::cout << " table_selec{ ";
            selected = -1;
            for(int i=0; i<table_selections.size(); i++) {
                // std::cout<<"["<<i<<"]:"<<std::boolalpha<<table_selections[i]<<" ";
                if(table_selections[i]) {
                    selected = i;
                    break;
                }
            }
            // std::cout<<"} ";
            if(selected!=-1) {
                // std::cout << "  selected:"<<selected<<"  table_selections.size(): "<<table_selections.size()<<"  _pwdFileCont.size():"<<_pwdFileCont.size()<<std::endl;
                valid_selection = validExtension(_pwdFileCont[selected].name, _valid__extensions);

                ImGui::SetCursorPos(padding__content_fileExplorer);
                ImGui::TextUnformatted(std::string(_pwdFileCont[selected].name).c_str());
                
                static std::string _strLabel_size   = formatNumber<std::string>("Size", 15, 0, "left");
                static std::string _strLabel_path   = formatNumber<std::string>("Path", 15, 0, "left");
                static std::string _strLabel_aTM    = formatNumber<std::string>("Last accessed", 15, 0, "left");
                static std::string _strLabel_mTM    = formatNumber<std::string>("Last modified", 15, 0, "left");
                // static std::string _strLabel_

                ImGui::SetCursorPosX(padding__content_fileExplorer.x);
                ImGui::TextUnformatted((
                    _strLabel_size + ": " +
                    formatBytes(_pwdFileCont[selected].size, 6, 2, false, "right")
                ).c_str());
                ImGui::SetCursorPosX(padding__content_fileExplorer.x);
                ImGui::TextUnformatted((
                    _strLabel_path + ": " +
                    currDir + std::string(_pwdFileCont[selected].name)
                ).c_str());
                ImGui::SetCursorPosX(padding__content_fileExplorer.x);
                ImGui::TextUnformatted((
                    _strLabel_aTM + ": " +
                    ctime(&_pwdFileCont[selected].atime)
                ).c_str());
                ImGui::SetCursorPosX(padding__content_fileExplorer.x);
                ImGui::TextUnformatted((
                    _strLabel_mTM + ": " +
                    ctime(&_pwdFileCont[selected].mtime)
                ).c_str());
                enterBarFileStr = _pwdFileCont[selected].name;
                if(areKeysPressed(std::vector<int>{662, 525}, &(guiKeys.pressed)) && !areKeysPressed(std::vector<int>{662, 525}, &(guiKeys.pressed), -2)) { //check if `mod-ctrl`+`enter` is pressed
                // if(isKeyPressed(525, &(guiKeys.pressed)) && !isKeyPressed(525, &(guiKeys.pressed), -2)) {
                    switch (_pwdFileCont[selected].type) {
                        case DT_REG:
                            if(valid_selection) {
                                _file__fileExplorer = currDir + std::string(_pwdFileCont[selected].name);
                                win_open = false;
                            }
                            break;
                        case DT_DIR:
                            if(std::string(_pwdFileCont[selected].name)==".") {}
                            else if(std::string(_pwdFileCont[selected].name)=="..") {
                                std::string _tempDir = "/";
                                for(int i=0; i<_parsed_path.size()-1; i++) _tempDir += _parsed_path[i] + "/";
                                newDirStr = _tempDir;
                                newDir = true;
                            }
                            else {
                                newDirStr = currDir + std::string(_pwdFileCont[selected].name) + "/";
                                newDir = true;
                            }

                            break;
                    }
                }
            }
            else {
                // std::cout << std::endl;
            }
            ImVec2 _currPos(padding__content_fileExplorer.x, _pos__sub_fileExplorer_detailsPanel(true)[2].y - ImGui::GetTextLineHeightWithSpacing() - padding__content_fileExplorer.y);
            ImGui::SetCursorPos(_currPos);
            ImGui::SetNextItemWidth((_pos__sub_fileExplorer_detailsPanel(false)[2].x-padding__content_fileExplorer.x)*0.6);
            if(ImGui::InputText("##fileInput", &enterBarFileStr, ImGuiInputTextFlags_EnterReturnsTrue)) { //sends true when enter is pressed

                mode2_inpCorrect = false;
                bool existingMatch = false;
                for(int i=0; i<_pwdFileCont.size(); i++) {
                    if(enterBarFileStr==_pwdFileCont[i].name) { //set every table_selections to false except the one that is entered by InputText
                        for(int ii=0; ii<table_selections.size(); ii++) { table_selections[ii] = (ii==i? true : false); }
                        existingMatch = true;
                        selected = i;
                        break;
                    }
                }

                if(_mode__fileExplorer==2 && enterBarFileStr.size()>0) {
                    valid_selection = validExtension(enterBarFileStr, _valid__extensions);

                    if(valid_selection && !existingMatch) {
                        selected = -1;
                        for(int ii=0; ii<table_selections.size(); ii++) { table_selections[ii] = false; }
                        mode2_inpCorrect = true;
                    }
                    else {

                    }
                }
            }
            _currPos.x += padding__content_fileExplorer.x + (_pos__sub_fileExplorer_detailsPanel(false)[2].x-padding__content_fileExplorer.x)*0.6;
            ImVec2 buttonSize((_pos__sub_fileExplorer_detailsPanel(false)[2].x - _currPos.x - padding__content_fileExplorer.x*2/*times two because spacing between buttons is padd[..]*/)*0.5, 0);
            ImGui::SetCursorPos(_currPos);
            
            ImGui::BeginDisabled(
                ((_mode__fileExplorer-1)%2==0 && !valid_selection) ||
                (_mode__fileExplorer%2==2 && (!mode2_inpCorrect && !valid_selection))
            );
            if(ImGui::Button(_mode__types[_mode__fileExplorer].c_str(), buttonSize)) {
                _file__fileExplorer = currDir + enterBarFileStr;
                win_open = false;
            }
            ImGui::EndDisabled();
            
            // ImGui::PopStyleVar();
            _currPos.x += buttonSize.x + padding__content_fileExplorer.x;
            ImGui::SetCursorPos(_currPos);
            if(ImGui::Button("Cancel", buttonSize)) {
                win_open = false;
            }

        }
        ImGui::EndChild();
        ImGui::SetCursorPos(_pos__sub_fileExplorer_navPanel()[0]);
        if(ImGui::BeginChild("navigaion_panel", _pos__sub_fileExplorer_navPanel(false)[2], child_flags)) {

        }
        ImGui::EndChild();

    }
    ImGui::End();

    if(newDir) currDir = newDirStr;

    // for(size_t i=0; i<_pwdFileCont.size(); i++) std::cout << " - " <<_pwdFileCont[i].name.c_str() << std::endl;

    // _mode__fileExplorer_prev = _mode__fileExplorer;
    if(!win_open) _mode__fileExplorer = 0;
}
