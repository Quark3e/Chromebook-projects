
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
        if(!tab0_schedule.loadFile("/home/berkhme/github_repo/Chromebook-projects/projects/proj_Hexclaw_cpp/gCode/ex0.nc")) {
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
        if(ImGui::BeginChild(
            "GCode_input",
            ImVec2(0, 0),
            ImGuiChildFlags_None
        )) {
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
    for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) {
        if (IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue;
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


void tab_1(void) {
    static bool init = true;

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::BeginGroup();
    // ImGui::Image()
    // al_draw_bitmap(bitmap_test, 5, 5, 0);
    
    // al_flip_display();
    // ImGui::Text("pointer = %x", my_image_texture);
    // ImGui::Text("size = %d x %d", my_image_width, my_image_height);
    // ImGui::Image((ImTextureID)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));

    // al_draw_indexed_prim()
    
    static std::vector<uint8_t> imgBits;
    static int imgSize[2] = {al_get_bitmap_width(bmpObj.BMP()), al_get_bitmap_height(bmpObj.BMP())};
    static std::unique_lock<std::mutex> u_lck_bmpObj(bmpObj.mtx, std::defer_lock);
    static std::unique_lock<std::mutex> u_lck_ndt(t_bitArr.imgMutex, std::defer_lock);

    if(init) {

        // u_lck_bmpObj.lock();
        // al_lock_mutex(th_allegMutex);
        imgSize[0] = al_get_bitmap_width(bmpObj.BMP());
        imgSize[1] = al_get_bitmap_height(bmpObj.BMP());
        // int x, y;
        // int cnt = 0;
        // for(int i=0; i<imgSize[0]*imgSize[1]*1; i++) {
        //     y = floor(i/imgSize[1]);
        //     x = i%imgSize[0];
        //     bmpObj.arr.push_back((float(x)/(imgSize[0]))*255);
        // }
        // u_lck_bmpObj.unlock();
        // // al_unlock_mutex(th_allegMutex);
        // bmpObj.newTask = true;
    }
    // mtx_print("main: request");
    u_lck_ndt.lock();
    u_lck_bmpObj.lock();
    if(t_bitArr.imgInit) {
        bmpObj.arr = t_bitArr.imgArr;
    }
    u_lck_bmpObj.unlock();
    u_lck_ndt.unlock();

    bmpObj.newTask = true;
    
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // while(bmpObj.newTask.load() && bmpObj.localRunning.load()) {
        // mtx_print("T0: waiting", false);
        // if(u_lck_bmpObj.try_lock()) {
        //     u_lck_bmpObj.unlock();
        //     break;
        // }
        // std::this_thread::sleep_for(std::chrono::milliseconds(2));
    // }
    // assert(loadBitmap_fromBitArray(bmpObj.BMP(), &(bmpObj.arr), "GRAY", bmpObj.width, bmpObj.height));

    if(_BM_DEFINE) Delays_table(perf_loadBitmap_func,"Delays table","Tab1 delays","Delays table");


    // al_set_target_bitmap(bitmap_test);
        // static float w, h, w1, h1;
        // static ALLEGRO_COLOR col = al_map_rgba(200, 50, 50, 220);
        // static uint8_t frameCount = 0;
        // frameCount++;
        // static bool evens = false;
        // static int  intCnt= 0;
        // if(frameCount>2) {
        //     evens = !evens;
        //     frameCount = 0; 
        //     for(int x_=intCnt*w1; x_<(intCnt+1)*w1; x_++) {
        //         for(int y_=intCnt*h1; y_<(intCnt+1)*h1; y_++) {
        //             al_put_pixel(x_, y_, al_map_rgba(0, 0, 0, 0));
        //         }
        //     }
        //     intCnt++;
        //     if(intCnt>=4) intCnt = 0;
        // }
        // if(init) {
        //     w = al_get_bitmap_width(bitmap_test);
        //     h = al_get_bitmap_height(bitmap_test);
        //     w1= w/4;
        //     h1= h/4;
        // }
        // for(int x_=intCnt*w1; x_<(intCnt+1)*w1; x_++) {
        //     for(int y_=intCnt*h1; y_<(intCnt+1)*h1; y_++) {
        //         al_put_pixel(x_, y_, col);
        //     }
        // }
    // al_set_target_backbuffer(display);
    
    

    while(bmpObj.newTask.load() && bmpObj.localRunning.load());

    // al_lock_mutex(th_allegMutex);
    // if(u_lck_bmpObj.try_lock()) {
    // u_lck_bmpObj.lock();
    ImGui::SetCursorPos(io.MousePos);
    ImGui::Image((ImTextureID)(intptr_t)bmpObj.BMP(), ImVec2(imgSize[0], imgSize[1]));
    //     u_lck_bmpObj.unlock();
    // }
    // else {
    //     mtx_print("T0: try lock failed: "+formatNumber<bool>(bmpObj.newTask.load(), 0, 0, "right"));
    // }
    
    // al_unlock_mutex(th_allegMutex);
    ImGui::EndGroup();
    if(init) init=false;
}
