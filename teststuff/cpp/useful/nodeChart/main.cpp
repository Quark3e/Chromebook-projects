
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



#include "gui_nodeChart.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>




int main(int argc, char** argv) {

    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_set_new_display_flags(!ALLEGRO_RESIZABLE);
    ALLEGRO_DISPLAY* display = al_create_display(1280, 720);
    al_set_window_title(display, "al window title");
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





    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    return 0;
}
