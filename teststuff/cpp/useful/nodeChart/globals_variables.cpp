

#include "globals_includes.hpp"


ImVec2 dim__main        = ImVec2(1280, 700);
ImVec2 dim__menu__node_detail = ImVec2(300, 500);

bool lockMove_screen    = true;
bool lockMove_node      = true;


int decay_mouseClick_left   = 0;
int decay_mouseClick_right  = 0;

int mouseAction_left    = -1;
int mouseAction_right   = -1;

bool mouseDrag_left      = false;

int mouseTimer_decay    = 20;


int static_mouseAction_left= -1;


