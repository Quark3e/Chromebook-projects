
#define WIN_HEIGHT  480
#define WIN_WIDTH   640

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>

#include <iostream>
#include <math.h>

int main(int argc, char** argv) {
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    ALLEGRO_DISPLAY* display = al_create_display(WIN_WIDTH, WIN_HEIGHT);
    al_set_window_title(display, "Dear Imgui Allegro5 Windows test");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    al_set_window_title(display, "Windows test window");


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


    ImGui::StyleColorsDark();

    ImGui_ImplAllegro5_Init(display);


    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    float my_color;

    bool running = true;

    bool my_tool_active = true;


    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_MenuBar;
    
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


        ImGui::Begin(" ", NULL, window_flags);
        ImGui::SetWindowPos(ImVec2(0, -19));
        ImGui::SetWindowSize(ImVec2(WIN_WIDTH, WIN_HEIGHT+19));


        if(ImGui::BeginMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Open..")) { /* Do stuff */ }
                if(ImGui::MenuItem("Save"))   { /* Do stuff */ }
                if(ImGui::MenuItem("Close"))  { my_tool_active = false; }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Program")) {
                if(ImGui::MenuItem("Exit"))   { running=false; }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Edit a color stored as 4 floats
        ImGui::ColorEdit4("Color", &my_color);

        // Generate samples and plot them
        float samples[100];
        for (int n = 0; n < 100; n++) samples[n] = sinf(n * 0.5f + ImGui::GetTime() * 1.5f);
        ImGui::PlotLines("Samples", samples, 100);

        // Display contents in a scrolling region
        ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");


        // ImGui::BeginGroup();
        ImGui::BeginChild("Scrolling", ImVec2(WIN_WIDTH/2, 100), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY);
        for (int n = 0; n < 100; n++) ImGui::Text("%04d: Some text", n);
        ImGui::EndChild();
        
        ImGui::SameLine();

        ImGui::BeginChild("Perf.", ImVec2(WIN_WIDTH/2, 100), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::EndChild();

        // ImGui::EndGroup();
        ImGui::End();

        //---------------------
        ImGui::Render();
        al_clear_to_color(al_map_rgba_f(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w));
        ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
        al_flip_display();
    }

    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    return 0;
}
