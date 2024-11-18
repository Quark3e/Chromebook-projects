
#include "../include/includes.hpp"


ALLEGRO_BITMAP *bitmap_test;

// int my_image_width = 0;
// int my_image_height = 0;
// GLuint my_image_texture = 0;


int mode = 0;


bool running = true;

float input_IK_pos[3]    = {0, 200, 150};
float input_IK_orient[3] = {0, 0, 0};

float output_IK_angles[6]= {0, 0, 0, 0, 0, 0}; //mtx dependant

// float output_FK_pos[3]   = {0, 0, 0};
// float output_FK_orient[3]= {0, 0, 0};

bool input_IK_enterPress = false;
bool keys__undo = false;
bool keys__redo = false;


bool _ctrl_enter__pressed = false; //`ctrl+enter`
bool _undo__pressed = false; //`ctrl+z`
bool _redo__pressed = false; //`ctrl+y` or `ctrl+shift+z`



DIY::typed_dict<std::string, std::string> guiSettings_desc= HW_KINEMATICS::setting_desc;
DIY::typed_dict<std::string, bool*> guiSettings;
// DIY::typed_dict<std::string, std::string> guiSettings

bool guisetting_link_to_server = true;
bool guisetting_findOrient = true;

bool takePerf_tab_0 = false;


