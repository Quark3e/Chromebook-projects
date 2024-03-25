
#pragma once
#ifndef HPP_HEXCLAW_OPTIONS
#define HPP_HEXCLAW_OPTIONS



#include "../../../../teststuff/cpp/useful/stringToFunc.hpp"



StringToFunction hexclaw_cmdArgs{};


void cmdArgs_info() {}

void option0() {}

void HW_setup_options() {
    hexclaw_cmdArgs.add_func("-h", cmdArgs_info, "--help");
    hexclaw_cmdArgs.add_func("-m", option0, "--main");



}





#endif