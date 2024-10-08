

#include "globals_includes.hpp"


std::string programCWD;

DIY::typed_dict<std::string, gNC::guiNodeChart> projects;


ImDrawList* project_draw_list = nullptr;


ImVec2 dim__main        = ImVec2(1280, 700);
ImVec2 dim__menu__detail= ImVec2(300, 600);

ImVec2 dim__menu__detail__offset    = ImVec2(20, 60);

bool lockMove_screen    = true;
bool lockMove_node      = true;


int decay_mouseClick_left   = 0;
int decay_mouseClick_right  = 0;

int mouseAction_left    = -1;
int mouseAction_right   = -1;

bool mouseDrag_left      = false;

int mouseTimer_decay    = 20;


int static_mouseAction_left= -1;


// int keyHistory_len = 100;
// std::vector<int> keyHistory;
// std::vector<int> keyHistory_mouseLeft;
