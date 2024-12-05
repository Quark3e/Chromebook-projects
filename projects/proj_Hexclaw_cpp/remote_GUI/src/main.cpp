/**
 * @file main.cpp
 * @author Erkhme Byambadorj (berkhme@gmail.com)
 * @brief Dear ImGui based remote gui control panel for the IK movement calculation and connection with hexclaw robot
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

#include "../include/includes.hpp"

// #include "textureHeader.h"

// static void glfw_error_callback(int error, const char* description) {
//     fprintf(stderr, "GLFW Error %d: %s\n", error, description);
// }

int main(int argc, char** argv) {

    if(argc>1) {
        for(int i=0; i<argc; i++) {
            if(strcmp(argv[i], "--APP")==0) {
                __OPEN_MODE = 1;
            }
        }
    }

    std::string path = getFileCWD(true);
    // std::cout<<path<<std::endl;
    mtx_print("T0:"+path);
    DIR* dir;
    struct dirent *ent;
    if((dir=opendir(path.c_str()))!=NULL) {
        while((ent=readdir(dir))!=NULL) {
            // std::cout<<ent->d_type<<": ";
            mtx_print("T0:"+std::string(ent->d_name));
            
        }
        closedir(dir);
    }
    else {
        perror("could not open directory");
        return EXIT_FAILURE;
    }
    mtx_print("");
    // for(const auto& entry: std::filesystem)

    guiSettings_desc.add("Link to server", std::string("whether to attempt to connect to Hexclaw Server at intervals"));
    guiSettings_desc.add("findOrient", std::string("whether to look for a valid orient if a given pos+orient gives invalid `getAngles` error"));

    std::list<bool*> vecPtr;
    for(const std::string key: HW_KINEMATICS::settings.keys()) vecPtr.push_back(HW_KINEMATICS::settings.getPtr(key));


    guiSettings = DIY::typed_dict<std::string, bool*>(HW_KINEMATICS::settings.keys(), vecPtr); //affects HW_KINEMATICS variables
    guiSettings.add("Link to server", &guisetting_link_to_server);
    guiSettings.add("findOrient", &guisetting_findOrient);


    // std::cout << "ALLEGRO_VERSION: " << ALLEGRO_VERSION_INT << std::endl;

    assert(al_init());
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    assert(al_init_image_addon());
    al_set_new_display_flags(!ALLEGRO_RESIZABLE | ALLEGRO_OPENGL_FORWARD_COMPATIBLE | ALLEGRO_OPENGL);
    display = al_create_display(WIN_WIDTH, WIN_HEIGHT);
    al_set_window_title(display, "Hexclaw remoteGUI");
    al_set_window_position(display, 0, 0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    // std::thread t_bmpLoader(threadTask_bitArrayProcess, &bmpObj);
    th_allegMutex  = al_create_mutex();
    assert(th_allegMutex);
    th_allegThread = al_create_thread(th_allegFunc, &bmpObj);
    assert(th_allegThread);
    al_start_thread(th_allegThread);
    // glfwSetErrorCallback(glfw_error_callback);
    // if (!glfwInit()) {
    //     return 1;
    // }
    // bool ret = LoadTextureFromFile("/home/berkhme/github_repo/Chromebook-projects/projects/proj_Hexclaw_cpp/remote_GUI/media/MyImage01.jpg", &my_image_texture, &my_image_width, &my_image_height);
    // IM_ASSERT(ret);


    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    // bitmap_test = al_load_bitmap("/home/berkhme/github_repo/Chromebook-projects/projects/proj_Hexclaw_cpp/remote_GUI/media/MyImage01.jpg");
    bitmap_test = al_create_bitmap(300, 300);
    bmpObj.init();

    al_set_new_bitmap_flags(!ALLEGRO_MEMORY_BITMAP);
    

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


    while(running.load()) {
        ALLEGRO_EVENT al_event;
        while(al_get_next_event(queue, &al_event)) {
            ImGui_ImplAllegro5_ProcessEvent(&al_event);
            if(al_event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                running = false;
                mtx_print("T0: closing: "+formatNumber<bool>(running.load(), 0, 0));
            }
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
            if(ImGui::BeginTabItem("Remote connect")) {
                tab_1();
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

    mtx_print("T0:join before.");
    // t_bmpLoader.join();
    al_join_thread(th_allegThread, NULL);
    al_destroy_thread(th_allegThread);
    mtx_print("T0:join after.");

    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
    al_destroy_bitmap(bitmap_test);
    bmpObj.destroy();
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    // glfwTerminate();

    return 0;
}


