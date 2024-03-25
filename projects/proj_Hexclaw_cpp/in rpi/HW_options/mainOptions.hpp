
#pragma once
#ifndef HPP_HEXCLAW_OPTIONS
#define HPP_HEXCLAW_OPTIONS



#include "../../../../teststuff/cpp/useful/stringToFunc.hpp"
#include "../../../../teststuff/cpp/useful/createTable.hpp"



StringToFunction hexclaw_cmdArgs{};
createTable cmdArgs_output_table;

/// @brief display all the command line argument flags with their descriptions
void cmdArgs_info() {
    cmdArgs_output_table = createTable(3, hexclaw_cmdArgs.getDescriptions().size());
    for(size_t i=0; i<hexclaw_cmdArgs.getDescriptions().size(); i++) {
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getNames()[i][0], 0, 0);
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getNames()[i][1], 1, 0);
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getDescriptions()[i], 2, 0);
    }
    cmdArgs_output_table.strExport("",false,"\n","\t ");
    std::cout << cmdArgs_output_table.exportStr << std::endl;
}

/// @brief run main/default option
void HW_option0();
/// @brief run intro servo movement
void HW_option1();
/// @brief calibrate HSV values
void HW_option2();

void HW_option3();

void HW_setup_options() {
    hexclaw_cmdArgs.add_func("-h", cmdArgs_info, "--help", "show help message with info on flags");
    hexclaw_cmdArgs.add_func("-c", HW_option2, "--cal", "calibrate/setup \"Hue Saturation Value\" -values");
    hexclaw_cmdArgs.add_func("-m", HW_option0, "--main", "run main/default version with servo control and opencv based tracking");
    hexclaw_cmdArgs.add_func("-i", HW_option1, "--intro", "run only servo intro movement");
    // hexclaw_cmdArgs.add_func("-1", HW_option2, "--", "calibrate/setup \"Hue Saturation Value\" -values");
    
}


void HW_option0() {

}
void HW_option1() {

}
void HW_option2() {

}
void HW_option3() {

}



#endif