/**
 * @file main.cpp
 * @author Erkhme Byambadorj (berkhme@gmail.com)
 * @brief ImGui gui control panel for the IK movement, connectable to main
 * @version 0.1
 * @date 2024-06-01
 * 
 * @copyright Copyright (c) 2024
 * 
 * 
 * modes:
 *  Terminal:   [0] local -> remote/server: solved IK/FK - send to server if connected
 *  Main    :   [1] local <- remote/server: receive solved IK/FK from main loop on server
 * 
 * settings:
 *  link to server: default: true   desc: whether to attempt to connect to Hexclaw Server at intervals
 *  findOrient    : default: true   desc: whether to look for a valid orient if a given pos+orient gives invalid `getAngles` error
 *  "a1:frameX"   : default: true   desc: getAngles: "frame1X = frame1X * cos(b)"
 *  "a1:a1"       : default: true   desc: getAngles: "a1 = a1 * cos(b)"
 *  "q4:default"  : default: false  desc: getAngles: "q4 = atan(frame1X / frame1Y)"
 *  "q4:a1"       : default: true   desc: getAngles: "q4 = a1"
 * "q4:a1:b:minus": default: true   desc: getAngles: "if(b<0) q4 = 0-a1; else q4=a1"
 * "q4:a1:b1:minus" default: true   desc: getAngles: "if(b1<0) q4 = 0-a1; else q4=a1"
 *  "q5:inPaper"  : default: false  desc: getAngles: "q5 = atan( [...] / (cos(b1) * cos(a1) ))"
 *  "q5:d5"       : default: false  desc: getAngles: "q5 = atan( [...] / (frame1X / tan(a1) ))"
 *  "exceedState" : default: true   desc: getAnlges: "if [...]_exceeded: positionIsReachable[0] = False"
 */


#include <iostream>
#include <math.h>
#include <stdint.h>

#include <HC_useful/useful.hpp>
#include <HC_useful/diy_dictionary.hpp>
#include <HC_useful/diy_performance.hpp>
#include <HC_useful/pathSchedule.hpp>
#include "../in rpi/IK_header.hpp"


#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>

#include <dirent.h>

#include <filesystem>
// namespace fs = std::filesystem;


#define WIN_WIDTH   600
#define WIN_HEIGHT  600

#define WIN_INPUT_IK_WIDTH  350
#define WIN_INPUT_IK_HEIGHT 100
#define WIN_INPUT_IK_POS  [0, 0]

#define WIN_INPUT_SETTINGS_WIDTH    200
#define WIN_INPUT_SETTINGS_HEIGHT   300
#define WIN_INPUT_SETTINGS_POS  [WIN_INPUT_IK_POS[0]+WIN_INPUT_IK_WIDTH, 0]

#define WIN_OUTPUT_ANGLES_WIDTH     350
#define WIN_OUTPUT_ANGLES_HEIGHT    100
#define WIN_OUTPUT_ANGLES_POS   [0, WIN_INPUT_IK_POS[1]+WIN_INPUT_IK_HEIGHT]

#define WIN_OUTPUT_FK_WIDTH     350
#define WIN_OUTPUT_FK_HEIGHT    100
#define WIN_OUTPUT_FK_POS   [0, WIN_OUTPUT_ANGLES_POS[1]+WIN_INPUT_IK_HEIGHT]


#define LIM_SHORTCUT_KEYS   8 //number of frames to wait after calling a boolean via key shortcut

#define MAX_HISTORY_SIZE    10

int mode = 0;


static void HelpMarker(const char* desc, const char* symb="(?)") {
    ImGui::TextDisabled(symb);
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


bool running = true;

static float input_IK_pos[3]    = {0, 200, 150};
static float input_IK_orient[3] = {0, 0, 0};

static float output_IK_angles[6]= {0, 0, 0, 0, 0, 0}; //mtx dependant

static float output_FK_pos[3]   = {0, 0, 0};
static float output_FK_orient[3]= {0, 0, 0};

bool input_IK_enterPress = false;
bool keys__undo = false;
bool keys__redo = false;


bool _ctrl_enter__pressed = false; //`ctrl+enter`
bool _undo__pressed = false; //`ctrl+z`
bool _redo__pressed = false; //`ctrl+y` or `ctrl+shift+z`


// struct funcs {
static bool IsLegacyNativeDupe(ImGuiKey key) {
    return key >= 0 && key < 512 && ImGui::GetIO().KeyMap[key] != -1;
}
// }; // Hide Native<>ImGuiKey duplicates when both exists in the array

DIY::typed_dict<std::string, std::string> guiSettings_desc= HW_KINEMATICS::setting_desc;
DIY::typed_dict<std::string, bool*> guiSettings;
// DIY::typed_dict<std::string, std::string> guiSettings

static bool guisetting_link_to_server = true;
static bool guisetting_findOrient = true;

static bool takePerf_tab_0 = false;

/**
 * @brief Timeline/snapshot history "node"/change addition.
 * 
 * @tparam storeType - the type of variable stored in history vector
 * @param tmp reference to the temprorary object to add to the timeline. NOTE:this is meant to be the changed/new variant
 * @param historyVec reference to the vector that stores the history/snapshots
 * @param idx index of where on the history/snapshot vector that's currently indexed/used/displayed
 * @param _MAX maximum size of the history vector that when the length is beyond, will delete the oldest snapshot
 * @note if `idx` isn't at the end of the vector `historyVec` then this function will cut and delete everything after `idx` and add
 * then change from there, essentially pruning the branch
 */
template<typename storeType>
void basic_timeline_add(storeType& tmp, std::vector<storeType>& historyVec, int& idx, int _MAX) {
    //Timeline change: addition
    if(idx>=_MAX) { //timeline addition: same branch: beyond MAX lim
        historyVec.erase(historyVec.begin()); //delete oldest
        historyVec.push_back(tmp); //add new
    }
    else if(idx>=historyVec.size()-1) { //timeline addition: same branch: still whithin MAX lim
        historyVec.push_back(tmp);
        idx+=1;
    }
    else { //timeline addition: new branch creation:
        historyVec.erase(historyVec.begin()+idx+1, historyVec.end());
        historyVec.push_back(tmp);
        idx+=1;
    }
}

void Delays_table(PERF::perf_isolated& perfObj, const char* childID, const char* tableID, const char* sepText);

void tab_0(void);
void tab_1(void);

int main(int argc, char** argv) {

    std::string path = getFileCWD(true);
    std::cout<<path<<std::endl;
    DIR* dir;
    struct dirent *ent;
    if((dir=opendir(path.c_str()))!=NULL) {
        while((ent=readdir(dir))!=NULL) {
            // std::cout<<ent->d_type<<": ";
            std::cout<<ent->d_name<<"\n";
        }
        closedir(dir);
    }
    else {
        perror("could not open directory");
        return EXIT_FAILURE;
    }
    std::cout<<std::endl;
    // for(const auto& entry: std::filesystem)

    guiSettings_desc.add("Link to server", std::string("whether to attempt to connect to Hexclaw Server at intervals"));
    guiSettings_desc.add("findOrient", std::string("whether to look for a valid orient if a given pos+orient gives invalid `getAngles` error"));

    std::list<bool*> vecPtr;
    for(const std::string key: HW_KINEMATICS::settings.keys()) vecPtr.push_back(HW_KINEMATICS::settings.getPtr(key));


    guiSettings = DIY::typed_dict<std::string, bool*>(HW_KINEMATICS::settings.keys(), vecPtr); //affects HW_KINEMATICS variables
    guiSettings.add("Link to server", &guisetting_link_to_server);
    guiSettings.add("findOrient", &guisetting_findOrient);



    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_set_new_display_flags(!ALLEGRO_RESIZABLE);
    ALLEGRO_DISPLAY* display = al_create_display(WIN_WIDTH, WIN_HEIGHT);
    al_set_window_title(display, "Dear ImGui Allegro 5 test (homemade)");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());



    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplAllegro5_Init(display);


    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    static ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoTitleBar;


    while(running) {
        ALLEGRO_EVENT al_event;
        while(al_get_next_event(queue, &al_event)) {
            ImGui_ImplAllegro5_ProcessEvent(&al_event);
            if(al_event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) running = false;
            if(al_event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
                ImGui_ImplAllegro5_InvalidateDeviceObjects();
                al_acknowledge_resize(display);
                ImGui_ImplAllegro5_CreateDeviceObjects();
            }
        }
        ImGui_ImplAllegro5_NewFrame();
        ImGui::NewFrame();
        //---------------------
        ImGui::SetNextWindowSizeConstraints(ImVec2(WIN_WIDTH, WIN_HEIGHT), ImVec2(WIN_WIDTH, WIN_HEIGHT));
        ImGui::Begin(" ", NULL, window_flags);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(ImVec2(WIN_WIDTH, WIN_HEIGHT));

        if(ImGui::BeginMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Open")) { }
                if(ImGui::MenuItem("Save")) { }
                if(ImGui::MenuItem("Close")){ running=false; }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Program")) {
                if(ImGui::MenuItem("Exit")) { running=false; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if(ImGui::BeginTabBar("Tabs")) {
            if(ImGui::BeginTabItem("Control Panel")) {
                tab_0();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }


        ImGui::SetCursorPos(ImVec2(WIN_WIDTH*0+10, WIN_HEIGHT-25));
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
        //---------------------
        ImGui::Render();
        al_clear_to_color(al_map_rgba_f(clear_color.x*clear_color.w,clear_color.y*clear_color.w,clear_color.z*clear_color.w, clear_color.w));
        ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
        al_flip_display();
        std::cout.flush();
    }
    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    return 0;
}


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

}

void Delays_table(PERF::perf_isolated& perfRef, const char* childID, const char* tableID, const char* sepText="") {
    
    if(ImGui::BeginChild(childID, ImVec2(WIN_INPUT_SETTINGS_WIDTH, 200))) {
        ImGui::SeparatorText(sepText);
        if(ImGui::BeginTable(tableID, 2)) {
            std::vector<float> perf_delays = perfRef.delays();
            std::vector<std::string> perf_names = perfRef.names();
            for(int i=0; i<perf_names.size(); i++) {
                ImGui::TableNextRow();
                // ImGui::SetNextItemWidth(perf_name.length());
                ImGui::TableSetColumnIndex(0);
                // ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted(perf_names[i].c_str());
                ImGui::TableSetColumnIndex(1);
                // ImGui::TextUnformatted("test");
                // ImGui::SetNextItemWidth(5);
                // ImGui::TableSetColumnIndex(1);
                // ImGui::AlignTextToFramePadding();
                ImGui::Text((formatNumber<float>(perf_delays[i],5,2)+"ms").c_str());
                // if(i>=5) break;
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
    }
}
