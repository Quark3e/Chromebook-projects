
#include "imguiwin.hpp"


bool GUINC::inRegion(ImVec2 cursorPos, ImVec2 cornerA, ImVec2 cornerB, bool includeEdge) {
    if(cornerA.x==cornerB.x && cornerA.y==cornerB.y) throw std::invalid_argument("GUINC::inRegion(ImVec2, ImVec2, ImVec2) the coordinates of the corner's can't be the exact same.");
    ///cornerA will hold minimum values and cornerB will hold maximum values
    if(cornerA.x>cornerB.x) {
        float temp = cornerA.x;
        cornerA.x = cornerB.x;
        cornerB.x = temp;
    }
    if(cornerA.y>cornerB.y) {
        float temp = cornerA.y;
        cornerA.y = cornerB.y;
        cornerB.y = temp;
    }

    
    if(includeEdge) {
        if(cursorPos.x >= cornerA.x && cursorPos.x <= cornerB.x && cursorPos.y >= cornerA.y && cursorPos.y <= cornerB.y) return true;
        else return false;
    }
    else {
        if(cursorPos.x >  cornerA.x && cursorPos.x <  cornerB.x && cursorPos.y >  cornerA.y && cursorPos.y <  cornerB.y) return true;
        else return false;
    }
}


GUINC::imguiwin::imguiwin(bool _init) {
    if(__init) throw std::runtime_error("ERROR: imguiwin(bool) this library instance is already initialised.");
    
    if(_init) this->init();
}
GUINC::imguiwin::~imguiwin() {
    if(!__init) return;

    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    __running = false;
    if(callback_func__running_exit) callback_func__running_exit();
}

bool GUINC::imguiwin::init() {
    if(__init) {
        std::cout << "GUINC::imguiwin::init(): WARNING: member function 'init()' is called when this class instance has already been initialised." << std::endl;
        return true;
    }

    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    
    this->display = al_create_display(win_dim.x, win_dim.y);
    al_set_window_title(this->display, "imguiwin window");
    this->queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();


    ImGui_ImplAllegro5_Init(display);

    __running = true;
    __init = true;
    return __init;
}


void GUINC::imguiwin::newFrame() {
    if(!__init)         throw std::runtime_error("ERROR: newFrame called when library hasn't been initialised.");
    if(__frameStarted)  throw std::runtime_error("ERROR: a frame hasn't been called to end before trying to set a new frame.");
    if(!__running)      throw std::runtime_error("ERROR: the library is not running before newFrame is called. Likely the window has been closed.");

    ALLEGRO_EVENT al_event;
    while(al_get_next_event(queue, &al_event)) {
        ImGui_ImplAllegro5_ProcessEvent(&al_event);
        if(al_event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            __running = false;
            if(callback_func__running_exit) callback_func__running_exit();
            return;
        }
        if(al_event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
            ImGui_ImplAllegro5_InvalidateDeviceObjects();
            al_acknowledge_resize(display);
            ImGui_ImplAllegro5_CreateDeviceObjects();
            win_dim = ImVec2(al_get_display_width(display), al_get_display_height(display));
        }
    }

    ImGui_ImplAllegro5_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSizeConstraints(win_dim, win_dim);
    ImGui::Begin("Main", NULL, winFlags_main);
    ImGui::SetWindowPos(ImVec2(0, 0));

    __running = true;
    __frameStarted = true;
}
void GUINC::imguiwin::endFrame() {
    if(!__init)         throw std::runtime_error("ERROR: newFrame called when library hasn't been initialised.");
    if(!__frameStarted) throw std::runtime_error("ERROR: a new frame hasn't been started before calling to end it.");
    if(!__running)      throw std::runtime_error("ERROR: the library is not running before endFrame is called. Likely the window has been closed.");

    ImGuiIO io = ImGui::GetIO();
    
    ImGui::SetCursorPos(ImVec2(10, win_dim[1]-25-ImGui::GetTextLineHeightWithSpacing()*1));
    ImGui::Text("Mouse pos: x:%3.1f y:%3.1f", io.MousePos.x, io.MousePos.y);
    ImGui::SetCursorPosX(10);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f/io.Framerate, io.Framerate);
    ImGui::End();

    ImGui::Render();
    al_clear_to_color(al_map_rgba_f(clear_color.x*clear_color.w, clear_color.y*clear_color.w, clear_color.z*clear_color.w, clear_color.w));
    ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
    al_flip_display();

    __frameStarted = false;
}

const bool GUINC::imguiwin::running() {
    return __running;
}

ImDrawList* GUINC::imguiwin::draw() {
    if(!__init)         throw std::runtime_error("ERROR: newFrame called when library hasn't been initialised.");
    if(!__frameStarted) throw std::runtime_error("ERROR: a frame hasn't been started before trying to draw.");
    if(!__running)      throw std::runtime_error("ERROR: the library is not running before newFrame is called. Likely the window has been closed.");

    return ImGui::GetWindowDrawList();
}

void GUINC::imguiwin::exit() {
    __running = false;
}
