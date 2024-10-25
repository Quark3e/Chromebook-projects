
#pragma once
#ifndef HPP__GUI_NODECHART__CONSTANTS
#define HPP__GUI_NODECHART__CONSTANTS

#define _DEBUG false



#include "globals_includes.hpp"


/**
 * @brief Struct for holding each project info
 * 
 */
struct nc_proj{
    /**
     * @brief fullpath filename to the file assosciated with this project
     */
    std::string filename = "";
    /**
     * @brief whether the project has a file assosciated/linked to it.
     * 
     */
    bool fileLinked = false;
    gNC::guiNodeChart chart;
};

extern std::string programCWD;

extern DIY::typed_dict<std::string, nc_proj> projects;

/**
 * Pointer to main "global" ImDrawList for the currently show projects draw window
 * 
 */
extern ImDrawList* project_draw_list;


extern ImVec2 dim__main;
extern ImVec2 dim__menu__detail;

extern ImVec2 dim__menu__detail__offset;

/**
 * Coefficients/scalars to solve window dimensions of fileExplorer relative to
 * absolute/program-window dim
 */
extern ImVec2 _coef_dim__win_fileExplorer;
/**
 * @brief get the dimensions for file explorer window
 * 
 * @return ImVec2 of x,y values
 */
ImVec2 dim__win_fileExplorer();

/**
 * The offset/padding of the contents to the sides/walls
 * 
 */
extern ImVec2 padding__content_fileExplorer;
std::vector<ImVec2>& _pos__sub_fileExplorer_addressBar(bool _update = true);
std::vector<ImVec2>& _pos__sub_fileExplorer_searchBar(bool _update = true);
std::vector<ImVec2>& _pos__sub_fileExplorer_headings(bool _update = true);
std::vector<ImVec2>& _pos__sub_fileExplorer_contents(bool _update = true);
std::vector<ImVec2>& _pos__sub_fileExplorer_detailsPanel(bool _update = true);
std::vector<ImVec2>& _pos__sub_fileExplorer_navPanel(bool _update = true);

/**
 * Coefficients/scalars to solve the window dimensions of fileExplorer relative to
 * absolute/program-window dim.
 * 
 * If values are bigger than `1`  then it'll use those values as actual dimensions
 */
extern ImVec2 _coef_dim__win_timeline;
/**
 * @brief width/height dimensions for the timeline window.
 * By default the window is placed at the bottom row so position is always y_bottom-`height`
 * 
 * @return ImVec2  (width, height)
 */
ImVec2 dim__win_timeline();

extern bool lockMove_screen;
extern bool lockMove_node;

// State checks
/**
 * Left_mouse click decay lock
 */
extern int decay_mouseClick_left;
/**
 * Right_mouse click decay lock
 */
extern int decay_mouseClick_right;

/**
 * Global variable to hold current left mouse button action:
 *  `-1` default: no action
 *  ` 0` screen (drag mouse: move)
 *  ` 1` node main
 *  ` 2` node connection (left mouse: drag)
 *  ` 3` link main
 */
extern int mouseAction_left;

/**
 * Global variable to hold current right mouse button action
 *  `-1` default: no action
 *  ` 0` screen (default rightClick menu)
 *  ` 1` node (node rightClick menu)
 *  ` 2` node connection (node connection rightClick menu)
 *  ` 3` link (link rightClick menu)
 */
extern int mouseAction_right;

/**
 * whether left button click is dragging (left click is pressed and mousec coords are changing)
 */
extern bool mouseDrag_left;

/// @brief rate at which the mouse lock value decays per iteration/frame 
extern int mouseTimer_decay;

/**
 * Global variable to hold the current mouse action in an absolute state:
 *  i.e. it's the same as `mouseAction_left`  but the state changes only if the new state isn't registered 
 *  as `-1`, meaning this variable will hold the previous state even if the button is no longer pressed
 * 
 */
extern int static_mouseAction_left;


std::vector<std::string> __parsePath(std::string __path, char sep_symb='/');
int __getPWD_content(std::string __path, std::vector<dirent*>& _retur);
int __getPWD_stats(std::string __path, std::vector<dirent*>& _dirents, std::vector<struct stat>& _retur);


std::string formatBytes(off_t _storage, int width=0, int decimals=1, bool use_sciNot=false, std::string align="right");

#endif