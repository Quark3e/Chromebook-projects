#pragma once
#ifndef HPP__LIB_imguiwin
#define HPP__LIB_imguiwin

#include "imguiwin__includes.hpp"
#include "pos2d.hpp"


namespace GUINC {
    
    /**
     * @brief Check if a cursor's coordinate position is located within/inside a bounding box with it's opposite corners given
     * 
     * @param cursorPos 2d space coordinate of the cursor position
     * @param cornerA 2d space coordinate of the bounding boxes corner located opposite to the other corner arg.
     * @param cornerB 2d space coordinate of the bounding boxes corner located opposite to the other corner arg.
     * @param includeEdge whether to include the edge of the bounding box when checking if cursorPos is in bounding box region.
     * @return true if the cursorPos 2d coordinate is located withing the bounding box.
     * @return false if the cursorPos is not located withing the bounding box.
     */
    bool inRegion(ImVec2 cursorPos, ImVec2 cornerA, ImVec2 cornerB, bool includeEdge=true);

    template<typename _VAR>
    inline ImVec2 toImVec2(pos2d<_VAR> toConv) {
        return ImVec2(float(toConv.x), float(toConv.y));
    }

    inline pos2d<float> toPos2d(ImVec2 toConv) {
        return pos2d<float>(toConv.x, toConv.y);
    }

    inline ImVec2 win_dim{DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT};


    class imguiwin {
    private:
        ALLEGRO_DISPLAY*        __display = nullptr;
        ALLEGRO_EVENT_QUEUE*    __queue;

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
            ImGuiWindowFlags_NoBringToFrontOnFocus | 
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBackground
        ;
    
        imguiwin(bool _init=true, size_t _width=DEFAULT_WIN_WIDTH, size_t _height=DEFAULT_WIN_HEIGHT);
        ~imguiwin();
    
        bool init();
        
        void newFrame();
        void endFrame();

        const bool running();

        ImDrawList* draw();

        void exit();

        void (*callback_func__running_exit)(void) = nullptr;
        ImVec4 clear_color = ImVec4(0.20f, 0.20f, 0.2f, 0.88f);
    };
    
     

} // namespace imGUINC



#endif // HPP__LIB_imguiwin