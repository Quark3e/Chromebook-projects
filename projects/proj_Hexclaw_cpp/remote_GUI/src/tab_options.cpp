
#include "../include/includes.hpp"


void tab_0(void) {

    static bool tab0_init = false;
    static IK_PATH::GCODE_schedule tab0_schedule;

    /**
     * General history of the states of tab0_schedule with each change/"node" saved as an element.
     * For simplicities sake, if a change is made while `history_idx_tab0_schedule` isn't at the end, all the changes after current point will be erased, essentially "pruning" that branch.
     * [0]      = oldest snapshot
     * [history_idx_tab0_schedule] = currently "used"/"shown" history node/change.
     */
    static std::vector<IK_PATH::GCODE_schedule> history_tab0_schedule;
    static int history_idx_tab0_schedule = -1; //index for currently displayed/"used" history "node"

    if(!tab0_init) {
        std::cout << "loading default gCode file:" << std::endl;
        tab0_schedule.verbose = true;
        if(!tab0_schedule.loadFile(__DIR_PROJECT_GCODE+"ex0.nc")) {
            // perror("tab0_schedule failed to load")
        }
        history_tab0_schedule.push_back(tab0_schedule);
        history_idx_tab0_schedule++;
    }

    static PERF::perf_isolated perf_tab0;

    static ImGuiTableFlags table_flags_IK_out = ImGuiTableFlags_Borders;
    // table_flags_IK_out |= ImGuiTableFlags_BordersOuter;
    // table_flags_IK_out |= ImGuiTableFlags_SizingStretchSame;
    table_flags_IK_out |= ImGuiTableFlags_NoHostExtendX;
    table_flags_IK_out |= ImGuiTableFlags_SizingFixedSame;


    static ImGuiChildFlags input_child_flags = 0;
    // input_child_flags |= ImGuiChildFlags_ResizeX;


    static ImGuiInputTextFlags input_inputText_flags = 0;
    // input_inputText_flags |= ImGuiInputTextFlags_


    static std::string outMsg = "";
 
    // perf_tab0.set_T0("group:0"); //perf time_point:0
    ImGui::BeginGroup();
    if(takePerf_tab_0) perf_tab0.set_T0("T:IK_input"); //perf time_point:0
    if(ImGui::BeginChild("IK input", ImVec2(WIN_INPUT_IK_WIDTH, WIN_INPUT_IK_HEIGHT), input_child_flags)) {
        ImGui::SeparatorText("Input");
        // ImGui::InputFloat3("pos", input_IK_pos);

        ImGui::PushItemWidth(WIN_INPUT_IK_WIDTH/6+30);
        ImGui::InputFloat("x",&input_IK_pos[0],0.0f,0.0f,"%.2f");
        ImGui::SameLine(); ImGui::InputFloat("y",&input_IK_pos[1],0.0f,0.0f,"%.2f");
        ImGui::SameLine(); ImGui::InputFloat("z",&input_IK_pos[2],0.0f,0.0f,"%.2f");


        ImGui::InputFloat("a",&input_IK_orient[0],0.0f,0.0f,"%.2f");
        ImGui::SameLine(); ImGui::InputFloat("B",&input_IK_orient[1],0.0f,0.0f,"%.2f");
        ImGui::SameLine(); ImGui::InputFloat("Y",&input_IK_orient[2],0.0f,0.0f,"%.2f");

        // ImGui::InputFloat3("tilt", input_IK_orient);
        static float _lim_orient[2] = {-90, 90};
        for(float& elem: input_IK_orient)
            elem = (elem>_lim_orient[1] ? elem=_lim_orient[1] : elem<_lim_orient[0] ? elem=_lim_orient[0] : elem=elem);
        
        // ImGui::SetCursorPosX(float(WIN_INPUT_IK_POS[0]));
        // ImGui::SetCursorPos(ImVec2(WIN_INPUT_IK_POS[0], WIN_INPUT_IK_POS[1]+WIN_INPUT_IK_HEIGHT-50));
        ImGui::SetCursorPos(ImVec2(275, 75));
        if(ImGui::Button("Enter")) { input_IK_enterPress = true; }
        // else if(input_IK_enterPress) input_IK_enterPress = false;

        ImGui::EndChild();
        if(takePerf_tab_0) perf_tab0.set_T1("T:IK_input"); //perf time_point:1
    }
    if(takePerf_tab_0) perf_tab0.set_T0("T:IK_info"); //perf time_point:0
    if(ImGui::BeginChild("IK info", ImVec2(WIN_INPUT_IK_WIDTH, WIN_INPUT_SETTINGS_HEIGHT-WIN_INPUT_IK_HEIGHT), input_child_flags)) {
        ImGui::SeparatorText("IK info");

        // const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y+ImGui::GetFrameHeightWithSpacing();
        static char buff_ScrollingRegion[256];
        // static char buff_temp[256];
        // static std::vector<std::vector<std::string>> history_scrollingRegions;

        if(ImGui::BeginChild(
            "ScrollingRegion",
            ImVec2(0, WIN_INPUT_SETTINGS_HEIGHT-WIN_INPUT_IK_HEIGHT-50),
            ImGuiChildFlags_None//, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NavFlattened
        )) {
            int tab0_schedule_size_len = std::to_string(tab0_schedule.size()).length();
            for(int i=0; i<tab0_schedule.size(); i++) {
                ImGui::PushID(i);

                std::string tempStr = "";
                for(int ii=0; ii<tab0_schedule[i].size(); ii++) tempStr+=tab0_schedule[i][ii]+" ";
                // ImGui::TextUnformatted(std::string(">> "+tempStr).c_str());
                // ImGui::TextUnformatted(std::string(">> "+DIY::prettyPrint_vec1<std::string>(tab0_schedule[i])).c_str());
                // ImGui::TextUnformatted(tab0_schedule.get_raw(i).c_str());
                snprintf(buff_ScrollingRegion, IM_ARRAYSIZE(buff_ScrollingRegion), "%s", tempStr.c_str());
                // sprintf(buff_ScrollingRegion, "%s", tempStr.c_str());
                ImGui::TextUnformatted(DIY::formatNumber<int>(i, tab0_schedule_size_len).c_str());
                ImGui::SameLine();
                if(ImGui::InputText("", buff_ScrollingRegion, IM_ARRAYSIZE(buff_ScrollingRegion), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    if(tab0_schedule.replace(i, std::string(buff_ScrollingRegion))==0) { //tab0_schedule modification
                        basic_timeline_add<IK_PATH::GCODE_schedule>(tab0_schedule, history_tab0_schedule, history_idx_tab0_schedule, MAX_HISTORY_SIZE);

                    }
                    else {
                        std::cout << "incorrect new gcode line."<<std::endl;
                    }
                }
                ImGui::SameLine();
                if(ImGui::Button(std::string("X").c_str())) {
                    tab0_schedule.erase(i);
                    basic_timeline_add<IK_PATH::GCODE_schedule>(tab0_schedule, history_tab0_schedule, history_idx_tab0_schedule, MAX_HISTORY_SIZE);
                }


                if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    ImGui::SetDragDropPayload("DND", &i, sizeof(int));
                    ImGui::EndDragDropSource();
                }
                if(ImGui::BeginDragDropTarget()) {
                    if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND")) {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int payload_i = *(const int*)payload->Data;
                        tab0_schedule.swap(i, payload_i);

                        basic_timeline_add<IK_PATH::GCODE_schedule>(tab0_schedule, history_tab0_schedule, history_idx_tab0_schedule, MAX_HISTORY_SIZE);

                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::PopID();
            }
            ImGui::EndChild();
            if(keys__undo || _undo__pressed) {
                if(history_idx_tab0_schedule>0) {
                    history_idx_tab0_schedule--;
                    tab0_schedule = history_tab0_schedule[history_idx_tab0_schedule];
                }
                else {}
                keys__undo = false;
                _undo__pressed = false;
            }
            if(keys__redo || _redo__pressed) {
                if(history_idx_tab0_schedule<history_tab0_schedule.size()-1) {
                    history_idx_tab0_schedule++;
                    tab0_schedule = history_tab0_schedule[history_idx_tab0_schedule];
                }
                else {}
                keys__redo = false;
                _redo__pressed = false;
            }
        }
        ImGui::Separator();
        static char buff_gcode_input[256];
        if(ImGui::BeginChild("GCode_input", ImVec2(0, 0), ImGuiChildFlags_None)) {
            ImGui::PushID(0);
            ImGui::TextUnformatted(">>");

            ImGui::SameLine();
            if(ImGui::InputText("", buff_gcode_input, IM_ARRAYSIZE(buff_gcode_input), 0)) {

            }

            ImGui::SameLine();
            if(ImGui::Button("Add")) {
                if(tab0_schedule.add(std::string(buff_gcode_input))==0) {
                    basic_timeline_add<IK_PATH::GCODE_schedule>(tab0_schedule, history_tab0_schedule, history_idx_tab0_schedule, MAX_HISTORY_SIZE);
                    snprintf(buff_gcode_input, IM_ARRAYSIZE(buff_gcode_input), "");
                }
                else {

                }
            }
            ImGui::SameLine();
            static int _insert_idx = -1;
            if(ImGui::Button("Insert")) ImGui::OpenPopup("GCODE_insert_popup");
            if(ImGui::BeginPopup("GCODE_insert_popup")) {
                ImGui::TextUnformatted("Insert index:");
                static int _schedule_insert_success = -1;
                if(ImGui::InputInt(std::string("range:[0:"+std::to_string(tab0_schedule.size())+"]").c_str(), &_insert_idx,1,100)) {
                }
                ImGui::SameLine();
                if(ImGui::Button("insert")) {

                    if(_insert_idx>=0 && _insert_idx<tab0_schedule.size()) {
                        _schedule_insert_success = tab0_schedule.insert(_insert_idx, std::string(buff_gcode_input));
                    }
                    else _schedule_insert_success = -1;

                    if(_schedule_insert_success==0) basic_timeline_add<IK_PATH::GCODE_schedule>(tab0_schedule, history_tab0_schedule, history_idx_tab0_schedule, MAX_HISTORY_SIZE);
                    else _schedule_insert_success = -1;

                }
                ImGui::EndPopup();
            }


            ImGui::PopID();
            ImGui::EndChild();
        }

        ImGui::EndChild();
        if(takePerf_tab_0) perf_tab0.set_T0("T:IK_info"); //perf time_point:1
    }
    ImGui::EndGroup();
    // perf_tab0.set_T1("group:0"); //perf time_point: 1

    ImGui::SameLine();

    // perf_tab0.set_T0("group:1"); //perf time_point:0
    ImGui::BeginGroup();
    if(takePerf_tab_0) perf_tab0.set_T0("T:Settings"); //perf time_point:0
    if(ImGui::BeginChild("Settings", ImVec2(WIN_INPUT_SETTINGS_WIDTH, WIN_INPUT_SETTINGS_HEIGHT))) {
        ImGui::SeparatorText("Settings");

        std::vector<std::string> temp_keys = guiSettings.keys();
        std::list<bool*> temp_settingPtr = guiSettings.values();
        for(int _i=0; _i<temp_keys.size(); _i++) {
            ImGui::Checkbox(DIY::formatNumber<std::string>(temp_keys[_i],15,0,"left").c_str(), guiSettings[_i]);
            ImGui::SameLine();
            HelpMarker(guiSettings_desc[_i].c_str());
            ImGui::SameLine();
            // if(_i<HW_KINEMATICS::setting_default.size()) {
            //     HelpMarker(("default:["+))
            // }
            HelpMarker(
                std::string(
                    "default:["+
                    ((_i<HW_KINEMATICS::setting_default.size())? HW_KINEMATICS::setting_default.str_export(temp_keys[_i],5,0,"left",true) : std::string("true "))+"]"
                ).c_str(), std::string("[]").c_str()
            );
        }

        ImGui::EndChild();
        if(takePerf_tab_0) perf_tab0.set_T1("T:Settings"); //perf time_point:1
    }
    ImGui::EndGroup();
    // perf_tab0.set_T1("group:1"); //perf time_point:1
    //-------------
    // outMsg = "";

    if(takePerf_tab_0) perf_tab0.set_T0("T:Keys"); //perf time_point:0
    /**
     * 515 [TAB]
     * 568 [w]
     * 525 [ENTER], 
     * 527 [L_CTRL]
     * 528 [L_SHIFT]
     * 529 [L_ALT]
     * 
     * 531 [R_CTRL]
     * 532 [R_SHIFT]
     * 533 [R_ALT / alt-gr]
     * 
     * 569 [x]
     * 570 [y]
     * 571 [z]
     * 
     * 
    */
    ImGuiKey start_key = (ImGuiKey)0;
    // std::cout << "----"<<start_key << std::endl;
    // int key_


    // int _keys_count_enter   = 0; //`ctrl+enter`
    // int _keys_count_exit    = 0; //`ctrl+x`
    // int _keys_count_undo    = 0; //`ctrl+z`
    // int _keys_count_redo    = 0; //`ctrl+y` or `ctrl+shift+z`
    
    static int _wait_keys__enter = 0;
    static int _wait_keys__undo = 0;
    static int _wait_keys__redo = 0;

    static std::vector<int> keys_enter  {527, 662, 525};
    static std::vector<int> keys_exit   {527, 662, 568};
    static std::vector<int> keys_undo   {527, 662, 571};
    static std::vector<int> keys_redo   {527, 662, 570};
    static std::vector<int> keys_redo1  {527, 528, 662, 663, 571};

    static std::vector<int> pressed_keys;
    pressed_keys.clear();
    for (ImGuiKey key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) {
        if (/*IsLegacyNativeDupe(key) ||*/ !ImGui::IsKeyDown(key)) continue;
        pressed_keys.push_back(key);
        // ImGui::SameLine();
        // ImGui::Text((key < ImGuiKey_NamedKey_BEGIN) ? "\"%s\"" : "\"%s\" %d", ImGui::GetKeyName(key), key);
        // if(key==527 || key==531) { //`L_ctrl`/`R_ctrl`
        //     _keys_count_enter++;
        //     _keys_count_exit++;
        //     _keys_count_undo++;
        //     _keys_count_redo++;
        // }
        // if(key==528 || key==532) { //`L_shift`/`R_shift`
        //     _keys_count_redo++;
        //     _keys_count_undo--;
        // }
        // if(key==525) _keys_count_enter++;   //`enter`
        // if(key==568) _keys_count_exit++;    //`w`
        // if(key==571) _keys_count_undo++;    //`z`
        // if(key==570) _keys_count_redo++;    //`y`
    }
    size_t size_pressed_keys = pressed_keys.size();
    if(size_pressed_keys==3) {
        if(match_vectors<int>(pressed_keys, keys_exit)) running = false;
        if(match_vectors<int>(pressed_keys, keys_enter) && _wait_keys__enter==0) {
            _ctrl_enter__pressed = true;
            _wait_keys__enter = 1;
        }
        else _ctrl_enter__pressed = false;
        if(match_vectors<int>(pressed_keys, keys_undo) && _wait_keys__undo==0) {
            _undo__pressed = true;
            _wait_keys__undo = 1;
        }
        else _undo__pressed = false;
        if(match_vectors<int>(pressed_keys, keys_redo) && _wait_keys__redo==0) {
            _redo__pressed = true;
            _wait_keys__redo = 1;
        }
        else _redo__pressed = false;
    }
    else if(size_pressed_keys==5) {
        if(match_vectors<int>(pressed_keys, keys_redo1) && _wait_keys__redo==0) {
            _redo__pressed = true;
            _wait_keys__redo = 1;
        }
        else _redo__pressed = false;
    }
    // if(size_pressed_keys!=0) std::cout<<DIY::prettyPrint_vec1<int>(pressed_keys)<<std::endl;
    // // std::cout << std::endl;
    // if(_keys_count_exit==2) running = false;
    // // if(_keys_count_enter==2 && !_ctrl_enter__pressed) input_IK_enterPress = true;
    // if(_keys_count_enter==2 && _wait_keys__enter==0) {
    //     _ctrl_enter__pressed  = true;
    //     _wait_keys__enter = 1;
    // }
    // else _ctrl_enter__pressed = false;
    // if(_keys_count_undo==2 && _wait_keys__undo==0) {
    //     _undo__pressed = true;
    //     _wait_keys__undo = 1;
    // }
    // else _undo__pressed = false;
    // if(_keys_count_redo==2 && _wait_keys__redo==0) {
    //     _redo__pressed = true;
    //     _wait_keys__redo = 1;
    // }

    if(_wait_keys__enter>=LIM_SHORTCUT_KEYS) _wait_keys__enter=0;
    else if(_wait_keys__enter>0) _wait_keys__enter++;

    if(_wait_keys__undo>=LIM_SHORTCUT_KEYS) _wait_keys__undo=0;
    else if(_wait_keys__undo>0) _wait_keys__undo++;

    if(_wait_keys__redo>=LIM_SHORTCUT_KEYS) _wait_keys__redo=0;
    else if(_wait_keys__redo>0) _wait_keys__redo++;

    if(takePerf_tab_0) perf_tab0.set_T1("T:Keys"); //perf time_point:1

    if(takePerf_tab_0) perf_tab0.set_T0("T:IK_group"); //prf time_point:0
    if(input_IK_enterPress || _ctrl_enter__pressed) {
        if(HW_KINEMATICS::getAngles(
                output_IK_angles,
                input_IK_pos,
                HW_KINEMATICS::toRadians(input_IK_orient[0]),
                HW_KINEMATICS::toRadians(input_IK_orient[1]),
                HW_KINEMATICS::toRadians(input_IK_orient[2]),
                1
        )) {
            outMsg = "found solution for given pos and orient";
        }
        else if(*guiSettings.get("findOrient")) {
            if(HW_KINEMATICS::findValidOrient(input_IK_pos, input_IK_orient, input_IK_orient, output_IK_angles)) { //find replacement
                outMsg = "note: No solution found for given orient:\nfound solution for different orient.";
            }
            else { //couldn't find replacement
                outMsg = "error: No solution for any possible\norient found for given pos.";
            }
        }
        else { outMsg = "error: No solution found."; }
        input_IK_enterPress = false;
        _ctrl_enter__pressed = false;
    }
    if(takePerf_tab_0)  perf_tab0.set_T1("T:IK_group"); //perf time_point:1
    
    
    //-------------

    // perf_tab0.set_T0("group:2"); //perf time_point:0
    ImGui::BeginGroup();
    if(takePerf_tab_0) perf_tab0.set_T0("T:IK_sol"); //perf time_point:0
    if(ImGui::BeginChild("Solved angles[deg.]", ImVec2(WIN_OUTPUT_ANGLES_WIDTH, WIN_OUTPUT_ANGLES_HEIGHT))) {
        ImGui::SeparatorText("IK solutions");
        ImGui::TextUnformatted(std::string(">> "+outMsg).c_str());
        ImGui::Separator();

        if(ImGui::BeginTable("IK_out", 6, table_flags_IK_out)) {
            for(int i=0; i<6; i++)
                ImGui::TableSetupColumn(std::string("q["+std::to_string(i)+"]").c_str());
            ImGui::TableHeadersRow();
            ImGui::TableNextRow();
            for(int i=0; i<6; i++) {
                ImGui::SetNextItemWidth(6);
                ImGui::TableSetColumnIndex(i);
                ImGui::AlignTextToFramePadding();
                ImGui::Text(formatNumber<float>(output_IK_angles[i],6,1).c_str());
            }

            ImGui::EndTable();
        }
        ImGui::EndChild();
        if(takePerf_tab_0) perf_tab0.set_T1("T:IK_sol"); //perf time_point:1
    }

    if(takePerf_tab_0) perf_tab0.set_T0("T:FK_sol"); //perf time_point;0
    if(ImGui::BeginChild("FK solutions", ImVec2(WIN_OUTPUT_FK_WIDTH, WIN_OUTPUT_FK_HEIGHT))) {
        ImGui::SeparatorText("FK solutions");


        ImGui::EndChild();
    }
    if(takePerf_tab_0) perf_tab0.set_T1("T:FK_sol"); //perf time_point:1
    ImGui::EndGroup();
    // perf_tab0.set_T1("group:2"); //perf time_point:1

    ImGui::SameLine();

    static int perf_maxString=0;
    // perf_maxString = stringOfVector(perf_tab0.names(),1);

    ImGui::BeginGroup();
    if(takePerf_tab_0) Delays_table(perf_tab0,"Delays table","Tab0 delays","Delays table");
    ImGui::EndGroup();

    tab0_init = true;
}

template<class _store_type>
struct status_check {
    _store_type _value;
    _store_type prev;

    bool doAutoPrevUpdate = true;
    bool doDiffChecking = true;
    /// @brief Difference check between current value and previous:
    /// current > prev : `1`.
    /// current < prev : `-1`.
    /// current == prev : `0`.
    int isDiff = 0;

    int diff(bool _updateInternal=true) {
        int diffVal = 0;
        if      (_value<prev)   diffVal = -1;
        else if (_value>prev)   diffVal = 1;
        else                    diffVal = 0;
        if(_updateInternal) this->isDiff = diffVal;
        return diffVal;
    }

    status_check(_store_type _init_value, _store_type _init_valuePrev, bool do_diff_checking=true, bool do_auto_prevUpdate=true)
    : _value(_init_value), prev(_init_valuePrev), doDiffChecking(do_diff_checking), doAutoPrevUpdate(do_auto_prevUpdate) {
        if(doDiffChecking) {
            if      (_value<prev)   isDiff = -1;
            else if (_value>prev)   isDiff = 1;
            else                    isDiff = 0;
        }
    }

    status_check operator=(_store_type _new_value) {
        if(doAutoPrevUpdate) this->prev      = _value;
        this->_value    = _new_value;
        if(doDiffChecking) {
            if      (_value<prev)   isDiff = -1;
            else if (_value>prev)   isDiff = 1;
            else                    isDiff = 0;
        }
        return *this;
    }
    status_check operator=(status_check &_ref) {
        if(doAutoPrevUpdate) this->prev      = _value;
        this->_value    = _ref._value;
        if(doDiffChecking) {
            if      (_value<prev)   isDiff = -1;
            else if (_value>prev)   isDiff = 1;
            else                    isDiff = 0;
        }
    }
    operator _store_type() const { return _value; }
    
    bool copy(status_check &_ref) {
        this->_value    = _ref._value;
        this->prev      = _ref.prev;
        this->doDiffChecking = _ref.doDiffChecking;
        this->isDiff    = _ref.isDiff;
    }
    void updatePrev() {
        this->prev      = this->_value;
        if(doDiffChecking) {
            if      (_value<prev)   isDiff = -1;
            else if (_value>prev)   isDiff = 1;
            else                    isDiff = 0;
        }
    }
    
    void callback() {
        // updatePrev();
        if(doDiffChecking) {
            if      (_value<prev)   isDiff = -1;
            else if (_value>prev)   isDiff = 1;
            else                    isDiff = 0;
        }
    }

    friend auto operator<<(std::ostream &os, status_check const& m) -> std::ostream& {
        os << "{"<<std::boolalpha<<m._value<<","<<std::boolalpha<<m.prev<<"}";
        return os;
    }
};
// template<class _classType>
void status_check__forwarder(void* context) {
    if(!context) return;
    static_cast<status_check<bool>*>(context)->callback();
}

void printFunc(float var, std::string& _ret) {
    _ret = formatNumber<float>(var, 5, 2, "right");
    // std::cout << "printFunc: " << _ret << std::endl;
}

void tab_1(void) {
    static bool init = true;

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::BeginGroup();
    
    static std::vector<uint8_t> imgBits;
    static int imgSize[2] = {al_get_bitmap_width(bmpObj.BMP()), al_get_bitmap_height(bmpObj.BMP())};
    static std::unique_lock<std::mutex> u_lck_bmpObj(bmpObj.mtx, std::defer_lock);
    static std::unique_lock<std::mutex> u_lck_ndt(t_bitArr.imgMutex, std::defer_lock);

    static status_check<bool> remote_videoFeed(false, false, true, false);
    static status_check<bool> remote_telemetry(false, false, true, false);

    static std::string  remote_videoFeed__IPADDR = "192.168.1.177";
    static int          remote_videoFeed__PORT   = DEFAULT_PORT;

    static std::string  remote_telemetry__IPADDR = DEFAULT__IPADDR;
    static int          remote_telemetry__PORT   = DEFAULT__PORT;
    static std::unique_lock<std::mutex> u_lck_remote_telemetry__telemData(telemetryObj.mtx_telemetry_data, std::defer_lock);
    // static bool remote_connect  = false;
    // static bool remote_connect_prev= false;
    // if(remote_connect != remote_connect_prev) {
    // std::cout << remote_videoFeed << std::endl;
    if(remote_videoFeed.diff()) {
        if(remote_videoFeed._value) { //the switch has been turned on: off->on
            if(!t_bitArr.imgInit.load()) { //check whether this class object has initialised (internally, not semantically)
                if(t_bitArr.func_init()) { // call init of t_bitArr. Returns true if an error has occurred

                    std::cerr << t_bitArr._info_name << " func_init() failed. code: "<<t_bitArr._error_code << std::endl;
                    remote_videoFeed = false;
                }
                else {
                    remote_videoFeed = true;
                }
            }
        }
        else { //switch has been turned off: on->off
            (&t_bitArr)->~NETWORK_DATA_THREADCLASS();
            new (&t_bitArr) NETWORK_DATA_THREADCLASS(false, remote_videoFeed__IPADDR, remote_videoFeed__PORT);
            std::cout << "cam has been turned off. Ntd object has been re-created" << std::endl;
        }
        // remote_connect_prev = remote_connect;
        remote_videoFeed.updatePrev();
    }
    if(remote_telemetry.diff()) {
        if(remote_telemetry._value) {
            // if(!orientObj.connectObj.isInit()) {
            //     if(orientObj.connectObj.init()) {
            //         std::cerr << "nodemcu_orient::nodemcu_connect::init() failed." << std::endl;
            //         remote_telemetry = false;
            //     }
            //     else {
            //         remote_telemetry = true;
            //         orientObj.accel._callFunc   = printFunc;
            //        orientObj.gyro._callFunc    = printFunc;
            //     }
            // }
            if(!telemetryObj.isInit()) {
                if(telemetryObj.init()) {
                    mtx_print("telemetryObj failed to initialise.");
                    // std::cout << "telemetryObj failed to initialise." << std::endl;
                    remote_telemetry = false;
                }
                else {
                    mtx_print("telemetryObj successfully initialised.");
                    // std::cout << "telemetryObj successfully initialised." << std::endl;
                    remote_telemetry = true;
                    telemetryObj.data_accelerometer._callFunc   = printFunc;
                    telemetryObj.data_gyroscope._callFunc       = printFunc;
                    telemetryObj.data_tilt._callFunc            = printFunc;
                    telemetryObj.data_tilt_RAW._callFunc        = printFunc;
                }
            }
        }
        else {
            // (&orientObj)->~nodemcu_orient();
            // new (&orientObj) nodemcu_orient("192.168.1.117", 1089, false);
            // orientObj.accel._callFunc   = printFunc;
            // orientObj.gyro._callFunc    = printFunc;
            // std::cout << "telemetry has been turned off. nodemcu_connect orientObj has been re-created." << std::endl;
            (&telemetryObj)->~threadClass_telemetry_receiver();
            new (&telemetryObj) threadClass_telemetry_receiver(remote_telemetry__IPADDR, remote_telemetry__PORT, false);
            telemetryObj.data_accelerometer._callFunc   = printFunc;
            telemetryObj.data_gyroscope._callFunc       = printFunc;
            telemetryObj.data_tilt._callFunc            = printFunc;
            telemetryObj.data_tilt_RAW._callFunc        = printFunc;
            mtx_print("telemetry has been turned off. threadClass_telemetry_receiver telemetryObj has been re-created.");
            // std::cout << "telemetry has been turned off. threadClass_telemetry_receiver telemetryObj has been re-created." << std::endl;
        }
        remote_telemetry.updatePrev();
    }

    if(init) {
        imgSize[0] = al_get_bitmap_width(bmpObj.BMP());
        imgSize[1] = al_get_bitmap_height(bmpObj.BMP());
    }
    if(t_bitArr.imgInit.load() && t_bitArr.runLoop.load()) {
        u_lck_ndt.lock();
        u_lck_bmpObj.lock();
        
        bmpObj.arr = t_bitArr.imgArr;
        
        bmpObj.newTask = true;
    
        u_lck_bmpObj.unlock();
        u_lck_ndt.unlock();
    }
    
    // if(telemetryObj.isInit()) {
    //     try {
    //         orientObj.update(false);
    //     }
    //     catch(const std::exception& e)
    //     {
    //         std::cerr << e.what() << '\n';
    //     }
    // }
    // std::cout << std::endl;
#if _BM_DEFINE
    Delays_table(perf_loadBitmap_func,"Delays table","Tab1 delays","Delays table");
#endif

    while(bmpObj.newTask.load() && bmpObj.localRunning.load());


    if(ImGui::BeginChild("Settings", ImVec2(0, WIN_HEIGHT-imgSize[1]*0.75-150))) {
        ImGui::SeparatorText("Settings");
        
        // ImGui::SetCursorPosX(0);
        ImGui::BeginTable("Settings", 3, ImGuiTableFlags_NoBordersInBody);
        ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
        ImGui::TableSetColumnIndex(0); ImGui::Text("Status");
        ImGui::TableSetColumnIndex(1); ImGui::Text("IP");
        ImGui::TableSetColumnIndex(2); ImGui::Text("PORT");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::PushID("videoFeed_status");  ToggleButton("", &remote_videoFeed._value, &status_check__forwarder, &remote_videoFeed);ImGui::PopID();
        ImGui::TableSetColumnIndex(1);
        ImGui::PushID("videoFeed_ip");      ImGui::InputText("", &remote_videoFeed__IPADDR);                                        ImGui::PopID();
        ImGui::TableSetColumnIndex(2);
        ImGui::PushID("videoFeed_port");    ImGui::InputInt("", &remote_videoFeed__PORT);                                           ImGui::PopID();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::PushID("telemetry_status");  ToggleButton("", &remote_telemetry._value, &status_check__forwarder, &remote_telemetry);ImGui::PopID();
        ImGui::TableSetColumnIndex(1);
        ImGui::PushID("telemetry_ip");      ImGui::InputText("", &remote_telemetry__IPADDR);                                        ImGui::PopID();
        ImGui::TableSetColumnIndex(2);
        ImGui::PushID("telemetry_port");    ImGui::InputInt("", &remote_telemetry__PORT);                                           ImGui::PopID();

        ImGui::EndTable();

        ImGui::EndChild();
    }
    if(ImGui::BeginChild("Data", ImVec2(0, imgSize[1]*0.75+50))) {
        ImGui::SeparatorText("Data");

        ImGui::Image((ImTextureID)(intptr_t)bmpObj.BMP(), ImVec2(imgSize[0]*0.5, imgSize[1]*0.5));
        if(telemetryObj.isInit()) {
            u_lck_remote_telemetry__telemData.lock();
            ImGui::Text(("Accel:"+std::string(telemetryObj.data_accelerometer)).c_str());
            ImGui::Text(("Gyro :"+std::string(telemetryObj.data_gyroscope)).c_str());
            ImGui::Text(("Tilt :"+std::string(telemetryObj.data_tilt)).c_str());
            u_lck_remote_telemetry__telemData.unlock();
        }
        ImGui::EndChild();
    }

    ImGui::EndGroup();
    if(init) init=false;
}
