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
#include "../in rpi/IK_header.hpp"


#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>


#define WIN_WIDTH   600
#define WIN_HEIGHT  600

#define WIN_INPUT_IK_WIDTH  350
#define WIN_INPUT_IK_HEIGHT 150
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


static float input_IK_pos[3]    = {0, 200, 150};
static float input_IK_orient[3] = {0, 0, 0};

static float output_IK_angles[6]= {0, 0, 0, 0, 0, 0}; //mtx dependant

static float output_FK_pos[3]   = {0, 0, 0};
static float output_FK_orient[3]= {0, 0, 0};

bool input_IK_enterPress = false;

struct funcs {
    static bool IsLegacyNativeDupe(ImGuiKey key) {
        return key >= 0 && key < 512 && ImGui::GetIO().KeyMap[key] != -1;
    }
}; // Hide Native<>ImGuiKey duplicates when both exists in the array





int main(int argc, char** argv) {

    static bool guisetting_link_to_server = true;
    static bool guisetting_findOrient = true;


    static DIY::typed_dict<std::string, std::string> guiSettings_desc= HW_KINEMATICS::setting_desc;
    guiSettings_desc.add("Link to server", std::string("whether to attempt to connect to Hexclaw Server at intervals"));
    guiSettings_desc.add("findOrient", std::string("whether to look for a valid orient if a given pos+orient gives invalid `getAngles` error"));

    std::vector<bool*> vecPtr;
    for(const std::string key: HW_KINEMATICS::settings.keys()) vecPtr.push_back(HW_KINEMATICS::settings.getPtr(key));


    DIY::typed_dict<std::string, bool*> guiSettings(HW_KINEMATICS::settings.keys(), vecPtr); //affects HW_KINEMATICS variables
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

    bool running = true;
    


    static ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoTitleBar;

    static ImGuiTableFlags table_flags_IK_out = ImGuiTableFlags_Borders;
    // table_flags_IK_out |= ImGuiTableFlags_BordersOuter;
    // table_flags_IK_out |= ImGuiTableFlags_SizingStretchSame;
    table_flags_IK_out |= ImGuiTableFlags_NoHostExtendX;
    table_flags_IK_out |= ImGuiTableFlags_SizingFixedSame;


    static ImGuiChildFlags input_child_flags = 0;
    // input_child_flags |= ImGuiChildFlags_ResizeX;


    static ImGuiInputTextFlags input_inputText_flags = 0;
    // input_inputText_flags |= ImGuiInputTextFlags_


    std::string outMsg = "";


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

        ImGui::BeginGroup();
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
            ImGui::SetCursorPos(ImVec2(250, 100));
            if(ImGui::Button("Enter")) { input_IK_enterPress = true; }
            else if(input_IK_enterPress) input_IK_enterPress = false;

            ImGui::EndChild();
        }
        if(ImGui::BeginChild("IK info", ImVec2(WIN_INPUT_IK_WIDTH, WIN_INPUT_SETTINGS_HEIGHT-WIN_INPUT_IK_HEIGHT), input_child_flags)) {
            ImGui::SeparatorText("IK info");


            ImGui::TextUnformatted(std::string(">> "+outMsg).c_str());

            ImGui::EndChild();
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        if(ImGui::BeginChild("Settings", ImVec2(WIN_INPUT_SETTINGS_WIDTH, WIN_INPUT_SETTINGS_HEIGHT))) {
            ImGui::SeparatorText("Settings");

            int _i = 0;

            for(std::string key: guiSettings.keys()) {
                ImGui::Checkbox(DIY::formatNumber<std::string>(key,15,0,"left").c_str(), guiSettings[key]);
                ImGui::SameLine();
                HelpMarker(guiSettings_desc[key].c_str());
                ImGui::SameLine();
                HelpMarker(
                    std::string(
                        "default:["+
                        ((_i<HW_KINEMATICS::setting_default.size())? HW_KINEMATICS::setting_default.str_export(key,5,0,"left",true) : std::string("true_"))+"]"
                    ).c_str(), std::string("[]").c_str()
                );
                _i++;
            }

            ImGui::EndChild();
        }
        ImGui::EndGroup();
        //-------------
        // outMsg = "";
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
        */
        ImGuiKey start_key = (ImGuiKey)0;
        int _ctrl_enter__count = 0;
        bool _ctrl_enter__pressed = false;
        for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) {
            if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue;
            // ImGui::SameLine();
            // ImGui::Text((key < ImGuiKey_NamedKey_BEGIN) ? "\"%s\"" : "\"%s\" %d", ImGui::GetKeyName(key), key);
            if(key==525 || key==527) _ctrl_enter__count++;
        }

        if(input_IK_enterPress || (_ctrl_enter__count==2 && !_ctrl_enter__pressed)) {
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
            else if(*guiSettings["findOrient"]) {
                if(HW_KINEMATICS::findValidOrient(input_IK_pos, input_IK_orient, input_IK_orient, output_IK_angles)) { //find replacement
                    outMsg = "note: No solution found for given orient:\nfound solution for different orient.";
                }
                else { //couldn't find replacement
                    outMsg = "error: No solution for any possible\norient found for given pos.";
                }
            }
            else { outMsg = "error: No solution found."; }
        }
        if(_ctrl_enter__count==2) _ctrl_enter__pressed = true;
        else _ctrl_enter__pressed = false;
        //-------------
        ImGui::BeginGroup();
        if(ImGui::BeginChild("Solved angles[deg.]", ImVec2(WIN_OUTPUT_ANGLES_WIDTH, WIN_OUTPUT_ANGLES_HEIGHT))) {
            ImGui::SeparatorText("IK solutions");
            if(ImGui::BeginTable("IK_out", 6, table_flags_IK_out)) {
                for(int i=0; i<6; i++) ImGui::TableSetupColumn(std::string("q["+std::to_string(i)+"]").c_str());
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
        }
        ImGui::EndGroup();

        if(ImGui::BeginChild("FK solutions", ImVec2(WIN_OUTPUT_FK_WIDTH, WIN_OUTPUT_FK_HEIGHT))) {
            ImGui::SeparatorText("FK solutions");
            ImGui::EndChild();
        }


        ImGui::SetCursorPos(ImVec2(WIN_WIDTH*0+10, WIN_HEIGHT-25));
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
        //---------------------
        ImGui::Render();
        al_clear_to_color(al_map_rgba_f(clear_color.x*clear_color.w,clear_color.y*clear_color.w,clear_color.z*clear_color.w, clear_color.w));
        ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
        al_flip_display();
    }
    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    return 0;
}
