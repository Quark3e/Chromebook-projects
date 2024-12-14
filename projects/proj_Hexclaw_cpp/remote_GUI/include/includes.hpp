
#pragma once
#ifndef HPP__HEXCLAW_REMOTE_GUI__INCLUDES
#define HPP__HEXCLAW_REMOTE_GUI__INCLUDES


#include <iostream>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include <cassert>

#include <dirent.h>
#include <filesystem>

#include <thread>
#include <mutex>
#include <atomic>

#include <chrono>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>


#include <useful.hpp>
#include <diy_dictionary.hpp>
#include <diy_performance.hpp>
#include <robotics/pathSchedule.hpp>
// #include <HC_useful/createTable.hpp>

#include <jpeglib.h>

// #include <NETWORKCLASS.hpp>
#include <hexclaw_networkDataThread.hpp>

// namespace fs = std::filesystem;
#include <IK_header.hpp>


#include "imgui_extras.hpp"
#include "constants.hpp"
#include "global_variables.hpp"
#include "tab_options.hpp"



#endif