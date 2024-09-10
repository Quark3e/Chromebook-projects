
/**
 * @file main.cpp
 * @author Erkhme Byambadorj (b.erkhme@gmail.com)
 * @brief ImGui based nodeChart
 * @version 0.1
 * @date 2024-08-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */



#include "globals_includes.hpp"


bool running_main = true;
bool opt__enable_grid = true;


int main(int argc, char** argv) {

    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    ALLEGRO_DISPLAY* display = al_create_display(dim__main.x, dim__main.y);
    al_set_window_title(display, "al window title");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); //(void)io;
    ImGuiStyle& style = ImGui::GetStyle();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


    ImGui::StyleColorsDark();
    ImGui_ImplAllegro5_Init(display);

    ImVec4 clear_color = ImVec4(0.20f, 0.20f, 0.2f, 0.88f);

    static ImGuiWindowFlags window0_flags   = 0;
    static ImGuiWindowFlags window1_flags   = 0;
    static ImGuiChildFlags  child0_flags    = 0;

    window0_flags |= ImGuiWindowFlags_NoMove;
    window0_flags |= ImGuiWindowFlags_NoResize;
    window0_flags |= ImGuiWindowFlags_NoCollapse;
    window0_flags |= ImGuiWindowFlags_MenuBar;
    window0_flags |= ImGuiWindowFlags_NoTitleBar;
    window0_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    

    window1_flags |= ImGuiWindowFlags_NoResize;


    gNC::guiNodeChart proj0;
    proj0.thisPtr = &proj0;

    static int cnt = 0;

    while(running_main) {
        static std::vector<int>* pressed_keys = update_keys();
        update_keys();

        ALLEGRO_EVENT al_event;
        while (al_get_next_event(queue, &al_event)) {
            ImGui_ImplAllegro5_ProcessEvent(&al_event);
            if(al_event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) running_main = false;
            if(al_event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
                ImGui_ImplAllegro5_InvalidateDeviceObjects();
                al_acknowledge_resize(display);
                ImGui_ImplAllegro5_CreateDeviceObjects();
                proj0.screen_dim[0] = al_get_display_width(display);
                proj0.screen_dim[1] = al_get_display_height(display);
                dim__main.x = proj0.screen_dim[0];
                dim__main.y = proj0.screen_dim[1];
            }
        }
        ImGui_ImplAllegro5_NewFrame();
        ImGui::NewFrame();
        //--------------------
        ImGui::SetNextWindowSizeConstraints(dim__main, dim__main);
        style.WindowRounding = 0;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(50, 50, 50, 0));
        ImGui::Begin(" ", NULL, window0_flags);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(dim__main);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // proj0.screen_pos[0]=cnt;
        // proj0.screen_pos[1]=cnt;

        if(ImGui::BeginMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Open")) { }
                if(ImGui::MenuItem("Save")) { }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Program")) {
                if(ImGui::MenuItem("Close")){ running_main = false; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if(ImGui::BeginTabBar("Tabs")) {
            if(ImGui::BeginTabItem("project 0")) {

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(50, 50, 50, 0));
                assert(ImGui::BeginChild("tab0: contents", ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_NoMove));
                ImGui::PopStyleColor();
                ImGui::PopStyleVar();
                
                if(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
                    if(!lockMove_screen && isKeyPressed(655, pressed_keys)) {
                        proj0.setScreen_pos(io.MouseDelta.x, io.MouseDelta.y, 1);
                        mouseDrag_left = true;
                    }
                    else {
                        mouseDrag_left = false;
                    }
                }

                ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
                ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
                if(canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
                if(canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
                ImVec2 canvas_p1 = ImVec2(canvas_p0.x+canvas_sz.x, canvas_p0.y+canvas_sz.y);


                static const int GRID_STEP = 64;
                if(opt__enable_grid) {
                    draw_list->PushClipRect(ImVec2(0, 20), ImVec2(proj0.screen_dim[0], proj0.screen_dim[1]), true);
                    for(float x=0; x<proj0.screen_dim[0]; x+=GRID_STEP)
                        draw_list->AddLine(ImVec2(x+(proj0.screen_pos[0] % GRID_STEP), 0+canvas_p0.y), ImVec2(x+(proj0.screen_pos[0]%GRID_STEP), canvas_p1.y), IM_COL32(200, 200, 200, 40));

                    for(float y=0; y<proj0.screen_dim[1]; y+=GRID_STEP)
                        draw_list->AddLine(ImVec2(canvas_p0.x, y+(proj0.screen_pos[1] % GRID_STEP)), ImVec2(canvas_p1.x, y+(proj0.screen_pos[1]%GRID_STEP)), IM_COL32(200, 200, 200, 40));
                    draw_list->PopClipRect();
                }


                if(cnt==0) {
                    proj0.NODE_create(100, 100, "node0", "desc0", "body0");
                    proj0.NODE_create(300, 300, "node1", "desc1", "body1");
                    proj0.NODE_create(500, 500, "node2", "desc2", "body2");
                    proj0.NODE_create(800, 300, "node3", "desc3", "body3");

                    proj0[1].bodyText = std::string("")+
                        "What is Lorem Ipsum? "+
                        "Lorem Ipsum is simply dummy text of the printing and typesetting industry. "+
                        "Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, "+
                        "when an unknown printer took a galley of type and scrambled it to make a type specimen book. "+
                        "It has survived not only five centuries, but also the leap into electronic typesetting, "+
                        "remaining essentially unchanged. "+
                        "It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, "+
                        "and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum."
                    ;

                    proj0.LINK_create(0, 2, 1, 0, "link0");
                    // proj0.LINK_create(&proj0[0], &proj0[2], 1, 0, "link0");
                    proj0.LINK_create(&proj0[1], &proj0[2], 3, 0, "link1");
                    proj0.LINK_create(&proj0[3], &proj0[1], 3, 2, "link2");

                    proj0.LINK_create(1, 3, 1, 4, "linkTest");

                    // proj0.NODE_delete(&proj0[1], true);

                }
                style.WindowRounding = 15.0f;
                proj0.draw();

                
                ImGui::EndChild();

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        cnt++;

        ImGui::SetCursorPos(ImVec2(10, dim__main[1]-25));
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f/io.Framerate, io.Framerate);
        ImGui::End();
        //--------------------
        ImGui::Render();
        al_clear_to_color(al_map_rgba_f(clear_color.x*clear_color.w, clear_color.y*clear_color.w, clear_color.z*clear_color.w, clear_color.w));
        ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
        al_flip_display();
        #if _DEBUG
        std::cout << "\n";
        std::cout.flush();
        #endif
    }

    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    return 0;
}

