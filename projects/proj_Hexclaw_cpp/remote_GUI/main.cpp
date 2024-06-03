/**
 * @file main.cpp
 * @author Erkhme Byambadorj (berkhme@gmail.com)
 * @brief ImGui gui control panel for the IK movement, connectable to main
 * @version 0.1
 * @date 2024-06-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>
#include <math.h>
#include <stdint.h>

#include <HC_useful/useful.hpp>
#include "../in rpi/IK_header.h"


#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>


#define WIN_WIDTH   500
#define WIN_HEIGHT  600

int main(int argc, char** argv) {

    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
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


    return 0;
}
