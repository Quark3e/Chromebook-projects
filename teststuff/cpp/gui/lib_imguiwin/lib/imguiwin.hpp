#pragma once
#ifndef HPP__LIB_imguiwin
#define HPP__LIB_imguiwin

#include "imguiwin__includes.hpp"
#include "pos2d.hpp"

namespace guiwin_nc {
    
    inline ImVec2 win_dim{DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};

    // using TDEF_void__ = void(*)();

    class imguiwin {
    private:
        ALLEGRO_DISPLAY*        display = nullptr;
        ALLEGRO_EVENT_QUEUE*    queue;

        bool __frameStarted = false;
        bool __init = false;
        bool __running = false;
    public:
        // imguiwin() = default;
        ImGuiWindowFlags winFlags_main =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoBringToFrontOnFocus;
    
        imguiwin(bool _init=true);
        ~imguiwin();
    
        bool init();
        
        void newFrame();
        void endFrame();

        const bool running();

        ImDrawList* draw();

        void (*callback_func__running_exit)(void) = nullptr;
        ImVec4 clear_color = ImVec4(0.20f, 0.20f, 0.2f, 0.88f);
    };
    
     

} // namespace imguiwin_nc



#endif // HPP__LIB_imguiwin