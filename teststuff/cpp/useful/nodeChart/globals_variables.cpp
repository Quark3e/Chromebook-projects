

#include "globals_includes.hpp"


ImVec2 dim__main        = ImVec2(1280, 700);

bool lockMove_screen    = true;
bool lockMove_node      = true;

//state checks: timer
int mouseClick_left     = 0;
int mouseClick_right    = 0;

/**
 * ID container for differentt mouse actions:
 * - `0` node screen
 * - `1` move node
 * - `2` node connection drag
 */
int mouseAction         = -1;
        
int mouseTimer_decay    = 20;


