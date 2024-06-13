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


#define WIN_WIDTH   500
#define WIN_HEIGHT  600

#define WIN_INPUT_IK_WIDTH  300
#define WIN_INPUT_IK_HEIGHT 200
#define WIN_INPUT_IK_POS  [0, 0]

#define WIN_INPUT_SETTINGS_WIDTH    200
#define WIN_INPUT_SETTINGS_HEIGHT   200
#define WIN_INPUT_SETTINGS_POS  [WIN_INPUT_IK_POS[0]+WIN_INPUT_IK_WIDTH, 0]

#define WIN_OUTPUT_ANGLES_WIDTH     600
#define WIN_OUTPUT_ANGLES_HEIGHT    100
#define WIN_OUTPUT_ANGLES_POS   [0, WIN_INPUT_IK_POS[1]+WIN_INPUT_IK_HEIGHT]

#define WIN_OUTPUT_FK_WIDTH     300
#define WIN_OUTPUT_FK_HEIGHT    100
#define WIN_OUTPUT_FK_POS   [0, WIN_OUTPUT_ANGLES_POS[1]+WIN_INPUT_IK_HEIGHT]


int mode = 0;

bool setting_findOrient = true;
bool setting_link_to_server = true;


int main(int argc, char** argv) {

    static DIY::typed_dict<std::string, std::string> guiSettings_desc= HW_KINEMATICS::setting_desc;
    guiSettings_desc.add("Link to server", std::string("whether to attempt to connect to Hexclaw Server at intervals"));
    guiSettings_desc.add("findOrient", std::string("whether to look for a valid orient if a given pos+orient gives invalid `getAngles` error"));

    
    std::vector<bool*> vecPtr;
    for(std::string key: HW_KINEMATICS::settings.keys()) vecPtr.push_back(HW_KINEMATICS::settings.getPtr(key));
    DIY::typed_dict<std::string, bool*> guiSettings(HW_KINEMATICS::settings.keys(), vecPtr);

    // static DIY::typed_dict<std::string, bool> guiSettings     = HW_KINEMATICS::settings;
    // guiSettings.add("Link to server", true);
    // guiSettings.add("findOrient", true);


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
    

    // const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    // const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

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
    input_child_flags |= ImGuiChildFlags_ResizeX;

    

    static float input_IK_pos[3]    = {0, 200, 150};
    static float input_IK_orient[3] = {0, 0, 0};

    static float output_IK_angles[6]= {0, 0, 0, 0, 0, 0};

    static float output_FK_pos[3]   = {0, 0, 0};
    static float output_FK_orient[3]= {0, 0, 0};



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
        ImGui::SetNextWindowSizeConstraints(ImVec2(WIN_WIDTH-1, WIN_HEIGHT-1), ImVec2(WIN_WIDTH, WIN_HEIGHT));
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


        if(ImGui::BeginChild("IK input", ImVec2(WIN_INPUT_IK_WIDTH, WIN_INPUT_IK_HEIGHT), input_child_flags)) {
            ImGui::SeparatorText("Input");
            ImGui::InputFloat3("pos", input_IK_pos);
            ImGui::InputFloat3("tilt", input_IK_orient);
            
            ImGui::EndChild();
        }
        ImGui::SameLine();
        if(ImGui::BeginChild("Settings", ImVec2(WIN_INPUT_SETTINGS_WIDTH, WIN_INPUT_SETTINGS_HEIGHT))) {
            ImGui::SeparatorText("Settings");

            ImGui::EndChild();
        }
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
                    // ImGui::TextUnformatted(formatNumber<float>(HW_KINEMATICS::solved_q[i],6,2).c_str());
                    ImGui::Text(formatNumber<float>(HW_KINEMATICS::solved_q[i],7,2).c_str());
                }

                ImGui::EndTable();
            }
            ImGui::EndChild();
        }
        if(ImGui::BeginChild("FK solutions", ImVec2(WIN_OUTPUT_FK_WIDTH, WIN_OUTPUT_FK_HEIGHT))) {
            ImGui::SeparatorText("FK solutions");
            ImGui::EndChild();
        }

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
