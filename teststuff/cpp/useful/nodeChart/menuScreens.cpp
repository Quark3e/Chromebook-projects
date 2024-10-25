
#include "globals_includes.hpp"


bool gNC::_winFocused__node_details = false;
bool gNC::_winFocused__link_details = false;


void gNC::_menu__node_details(
    gNC::gNODE* toDetail
) {
    assert(toDetail!=nullptr);
    static gNC::gNODE* init_node = nullptr;
    static bool init_prev = false;
    if(init_node) init_prev = true;

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
            // init_prev = false;
        }
        else {
            if(!init_prev) mouseAction_left = 1;
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
        if(ImGui::InputText("##_title", &(toDetail->label), inpText_flags_label | ImGuiInputTextFlags_EnterReturnsTrue)) {
            toDetail->inChart->modified = true;
        }
        
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        if(ImGui::DateChooser(("##"+ptrToStr<gNC::gNODE*>(toDetail)+"_date").c_str(), toDetail->date)) {
            toDetail->inChart->modified;
        }

        ImGui::Separator();
        ImGui::PushItemWidth(dim__menu__detail.x-_win_widthOffset);
        if(ImGui::InputText("##_desc", &(toDetail->desc), inpText_flags_desc | ImGuiInputTextFlags_EnterReturnsTrue)) {
            toDetail->inChart->modified = true;
        }
        
        ImGui::Separator();
        if(ImGui::InputTextMultiline("##_bodyText", &(toDetail->bodyText), ImVec2(dim__menu__detail.x-_win_widthOffset, ImGui::GetTextLineHeight()*5), inpText_flags_bodyT | ImGuiInputTextFlags_EnterReturnsTrue)) {
            toDetail->inChart->modified = true;
        }


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
        
    }
    ImGui::End();

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
        if(ImGui::InputText("##_title", &(toDetail->label), inpText_flags_label | ImGuiInputTextFlags_EnterReturnsTrue)) {
            toDetail->inChart->modified = true;
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        if(ImGui::DateChooser(("##"+ptrToStr<gNC::gLINK*>(toDetail)+"_date").c_str(), toDetail->date)) {
            toDetail->inChart->modified = true;
        }


        ImGui::Separator();
        ImGui::PushItemWidth(dim__menu__detail.x-_win_widthOffset);
        if(ImGui::InputText("##_desc", &(toDetail->desc), inpText_flags_desc | ImGuiInputTextFlags_EnterReturnsTrue)) {
            toDetail->inChart->modified = true;
        }

        ImGui::Separator();
        if(ImGui::InputTextMultiline("##_bodyText", &(toDetail->bodyText), ImVec2(dim__menu__detail.x-_win_widthOffset, ImGui::GetTextLineHeight()*5), inpText_flags_bodyT | ImGuiInputTextFlags_EnterReturnsTrue)) {
            toDetail->inChart->modified = true;
        }


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

    }
    ImGui::End();


    if(init_link!=toDetail) init_link = toDetail;
}


void gNC::_menu__rightClick(
    gNC::guiNodeChart* _chart
) {
    static bool init = false;

    static ImGuiIO& io = ImGui::GetIO();
    ImVec2 mousePos_temp = io.MousePos;

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

    static bool open    = true;
    static bool _init   = true;
    static bool _collapse   = true;
    static bool _collapse_prev  = true;

    _collapse_prev = _collapse;
    if(!open) {
        _collapse = true;
    }
    else {}

    open = true;
    static ImGuiWindowFlags win_flags = 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize;
    

    ImGui::Begin("Timeline", &open, win_flags);
    ImGui::SetWindowSize(dim__win_timeline());
    // ImGui::SetWindowCollapsed(_collapse);
    _collapse = ImGui::IsWindowCollapsed();

    if(_collapse != _collapse_prev | _init) {
        if(_collapse_prev) _init = true;
        //this iteration had a state change for _collapse
        ImGui::SetWindowPos(ImVec2(
            dim__main.x - ImGui::GetWindowSize().x,
            dim__main.y - ImGui::GetWindowSize().y
        ));
    }

    if(!_collapse) {

    }
    else {

    }
    ImGui::End();

    _init = false;
}


std::vector<std::string> __parsePath(std::string __path, char sep_symb) {
    assert(__path[0]==sep_symb);
    std::vector<std::string> _parsed;
    for(int i=0; i<__path.length(); i++) {
        if(__path[i]==sep_symb && i<__path.length()-1) {
            _parsed.push_back("");
            continue;
        }
        else if(__path[i]==sep_symb) break;
        _parsed[_parsed.size()-1] += __path[i];
    }
    return _parsed;
}

int __getPWD_content(
    std::string __path,
    std::vector<dirent*>& _retur
) {
    _retur.clear();
    DIR* dir;
    dirent* ent;
    if((dir=opendir(__path.c_str()))!=NULL) {
        while((ent=readdir(dir))!=NULL) {
            _retur.push_back(ent);
        }
        closedir(dir);
    }
    else {
        return -1;
    }
    return 0;
}

int __getPWD_stats(
    std::string __path,
    std::vector<dirent*>&       _dirents,
    std::vector<struct stat>&   _retur
) {
    _retur.clear();
    for(dirent* _ent : _dirents) {
        struct stat _temp;
        if(stat((__path+_ent->d_name).c_str(), &_temp)==-1) { //error occured
            return -1;
        }
        _retur.push_back(_temp);
    }

    return 0;
}


std::string gNC::_file__fileExplorer = "";
std::vector<std::string> gNC::_mode__types{"NULL", "Open", "Save", "Load", "Save As.."};
int         gNC::_mode__fileExplorer = 0;
int         gNC::_mode__fileExplorer_prev = 0;
std::vector<std::string> gNC::_valid__extensions;

void gNC::_menu__fileExplorer() {
    static const std::string defaultDir = "/home/berkhme/github_repo/Chromebook-projects/teststuff/cpp/useful/nodeChart/saveFiles/"; //getFileCWD(true);
    static std::string currDir = defaultDir;
    static std::string dir_history;
    
    static std::vector<dirent*>     _pwdCont;
    static std::vector<struct stat> _pwdCont_stat;
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
    if(!_mode__fileExplorer_prev) {
        isInit = true;
        //This is the "first"/init iteration of the new fileExplorer call.
        //This is the init call
        scr_width   = dim__win_fileExplorer().x;
        scr_height  = dim__win_fileExplorer().y;
        currDir     = defaultDir;
        selected    = -1;
        win_open    = true;
        _file__fileExplorer = "";
    }
    if(__getPWD_content(currDir, _pwdCont)) { //error occured
    }
    if(__getPWD_stats(currDir, _pwdCont, _pwdCont_stat)) { //error occured
    }

    /**
     * @brief container for the selected items/(files/folders) in current directory
     * @note needs to be updated whenever directory is changed
     */
    static std::vector<bool> table_selections(_pwdCont.size(), false);
    /**
     * @brief string that's stored/passed to inputtext fields for the selected files
     * @note needs to be updated whenever directory is changed
     */
    static std::string enterBarFileStr = "";
    static bool mode2_inpCorrect = false; //whether the input text in mode 2 is valid (to prevent it from disappearing)

    static std::vector<std::string> _parsed_path = __parsePath(currDir);

    if(!_mode__fileExplorer_prev || newDir) {
        table_selections = std::vector<bool>(_pwdCont.size(), false);
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
                    std::string _tempDir = "/";
                    for(int ii=0; ii<=i; ii++) _tempDir += _parsed_path[ii] + "/"; 
                    newDirStr = _tempDir;
                    newDir = true;
                }
                strPath_pos.x += _parsed_path[i].length()*(ImGui::GetFontSize()-6)+7;
                ImGui::PopID();
                ImGui::SetCursorPos(strPath_pos);
                ImGui::TextUnformatted("/");
                strPath_pos.x +=ImGui::GetFontSize()-5;
            }
            if(isInit) ImGui::SetScrollX(strPath_pos.x);

            ImGui::EndChild();
        }
        ImGui::SetCursorPos(_pos__sub_fileExplorer_searchBar()[0]);
        if(ImGui::BeginChild("search_bar", _pos__sub_fileExplorer_searchBar(false)[2], child_flags)) {

            ImGui::EndChild();
        }
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

                for(int i=0; i<_pwdCont.size(); i++) {
                    const bool item_is_selected = table_selections[i];

                    ImGui::PushID(i);
                    ImGui::TableNextRow();
                    if(ImGui::TableSetColumnIndex(0)) {
                        // ImGui::TextUnformatted(_pwdCont[i]->d_name);
                        ImGui::SetCursorPosX(padding__content_fileExplorer.x);
                        ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;
                        if(ImGui::Selectable(_pwdCont[i]->d_name, item_is_selected, selectable_flags, ImVec2(0, 0))) {
                            if(ImGui::GetIO().KeyCtrl) {
                                table_selections[i] = !item_is_selected;
                            }
                            else {
                                for(size_t ii=0; ii<table_selections.size(); ii++) table_selections[ii] = false;
                                table_selections[i] = true;
                            }
                            if(guiKeys.keyPeriod(655)>0) { //check if a file/folder has been double-clicked
                                switch (_pwdCont[i]->d_type) {
                                    case DT_REG:
                                        if(validExtension(_pwdCont[i]->d_name, _valid__extensions)) {
                                            _file__fileExplorer = currDir + std::string(_pwdCont[i]->d_name);
                                            win_open = false;
                                        }
                                        break;
                                    case DT_DIR:
                                        if(std::string(_pwdCont[i]->d_name)==".") {}
                                        else if(std::string(_pwdCont[i]->d_name)=="..") {
                                            std::string _tempDir = "/";
                                            for(int ii=0; ii<_parsed_path.size()-1; ii++) _tempDir += _parsed_path[ii] + "/";
                                            newDirStr = _tempDir;
                                            newDir = true;
                                        }
                                        else {
                                            newDirStr = currDir + std::string(_pwdCont[i]->d_name) + "/";
                                            newDir = true;
                                        }

                                        break;
                                }
                            }
                        }
                    }
                    if(ImGui::TableSetColumnIndex(1)) {
                        ImGui::TextUnformatted(formatBytes(_pwdCont_stat[i].st_size, 6, 2, false, "right").c_str());
                    }
                    if(ImGui::TableSetColumnIndex(2)) {
                        // ImGui::TextUnformatted((_pwdCont[i]->d_type==DT_REG? ))
                        std::string _type = "";
                        switch (_pwdCont[i]->d_type) {
                            case DT_REG: _type="file";  break;
                            case DT_DIR: _type="folder";break;
                        }
                        ImGui::TextUnformatted(_type.c_str());
                    }
                    if(ImGui::TableSetColumnIndex(3)) {
                        // ImGui::TextUnformatted(formatNumber<time_t>(_pwdCont_stat[i].st_mtime, 0, 0).c_str());
                        ImGui::TextUnformatted(ctime(&_pwdCont_stat[i].st_mtime));
                    }

                    ImGui::PopID();
                }
                ImGui::EndTable();
            }

            ImGui::EndChild();
        }
        ImGui::SetCursorPos(_pos__sub_fileExplorer_detailsPanel()[0]);
        if(ImGui::BeginChild("details_panel", _pos__sub_fileExplorer_detailsPanel(false)[2], child_flags)) {
            for(int i=0; i<table_selections.size(); i++) { if(table_selections[i]) { selected = i; break;} }

            if(selected!=-1) {
                valid_selection = validExtension(_pwdCont[selected]->d_name, _valid__extensions);

                ImGui::SetCursorPos(padding__content_fileExplorer);
                ImGui::TextUnformatted(std::string(_pwdCont[selected]->d_name).c_str());
                
                static std::string _strLabel_size   = formatNumber<std::string>("Size", 15, 0, "left");
                static std::string _strLabel_path   = formatNumber<std::string>("Path", 15, 0, "left");
                static std::string _strLabel_aTM    = formatNumber<std::string>("Last accessed", 15, 0, "left");
                static std::string _strLabel_mTM    = formatNumber<std::string>("Last modified", 15, 0, "left");
                // static std::string _strLabel_

                ImGui::SetCursorPosX(padding__content_fileExplorer.x);
                ImGui::TextUnformatted((
                    _strLabel_size + ": " +
                    formatBytes(_pwdCont_stat[selected].st_size, 6, 2, false, "right")
                ).c_str());
                ImGui::SetCursorPosX(padding__content_fileExplorer.x);
                ImGui::TextUnformatted((
                    _strLabel_path + ": " +
                    currDir + std::string(_pwdCont[selected]->d_name)
                ).c_str());
                ImGui::SetCursorPosX(padding__content_fileExplorer.x);
                ImGui::TextUnformatted((
                    _strLabel_aTM + ": " +
                    ctime(&_pwdCont_stat[selected].st_atime)
                ).c_str());
                ImGui::SetCursorPosX(padding__content_fileExplorer.x);
                ImGui::TextUnformatted((
                    _strLabel_mTM + ": " +
                    ctime(&_pwdCont_stat[selected].st_mtime)
                ).c_str());

                enterBarFileStr = _pwdCont[selected]->d_name;
                if(areKeysPressed(std::vector<int>{662, 525}, &(guiKeys.pressed)) && !areKeysPressed(std::vector<int>{662, 525}, &(guiKeys.pressed), -2)) { //check if `mod-ctrl`+`enter` is pressed
                // if(isKeyPressed(525, &(guiKeys.pressed)) && !isKeyPressed(525, &(guiKeys.pressed), -2)) {
                    switch (_pwdCont[selected]->d_type) {
                        case DT_REG:
                            if(valid_selection) {
                                _file__fileExplorer = currDir + std::string(_pwdCont[selected]->d_name);
                                win_open = false;
                            }
                            break;
                        case DT_DIR:
                            if(std::string(_pwdCont[selected]->d_name)==".") {}
                            else if(std::string(_pwdCont[selected]->d_name)=="..") {
                                std::string _tempDir = "/";
                                for(int i=0; i<_parsed_path.size()-1; i++) _tempDir += _parsed_path[i] + "/";
                                newDirStr = _tempDir;
                                newDir = true;
                            }
                            else {
                                newDirStr = currDir + std::string(_pwdCont[selected]->d_name) + "/";
                                newDir = true;
                            }

                            break;
                    }
                }
            }
            ImVec2 _currPos(padding__content_fileExplorer.x, _pos__sub_fileExplorer_detailsPanel(true)[2].y - ImGui::GetTextLineHeightWithSpacing() - padding__content_fileExplorer.y);
            ImGui::SetCursorPos(_currPos);
            ImGui::SetNextItemWidth((_pos__sub_fileExplorer_detailsPanel(false)[2].x-padding__content_fileExplorer.x)*0.6);
            if(ImGui::InputText("##fileInput", &enterBarFileStr, ImGuiInputTextFlags_EnterReturnsTrue)) { //sends true when enter is pressed

                mode2_inpCorrect = false;
                bool existingMatch = false;
                for(int i=0; i<_pwdCont.size(); i++) {
                    if(enterBarFileStr==std::string(_pwdCont[i]->d_name)) { //set every table_selections to false except the one that is entered by InputText
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

            ImGui::EndChild();
        }
        ImGui::SetCursorPos(_pos__sub_fileExplorer_navPanel()[0]);
        if(ImGui::BeginChild("navigaion_panel", _pos__sub_fileExplorer_navPanel(false)[2], child_flags)) {

            ImGui::EndChild();
        }

    }
    ImGui::End();

    if(newDir) currDir = newDirStr;

    // for(size_t i=0; i<_pwdCont.size(); i++) std::cout << " - " <<_pwdCont[i]->d_name << std::endl;

    // _mode__fileExplorer_prev = _mode__fileExplorer;
    if(!win_open) _mode__fileExplorer = 0;
}
