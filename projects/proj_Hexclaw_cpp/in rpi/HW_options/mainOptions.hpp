
#pragma once
#ifndef HPP_HEXCLAW_OPTIONS
#define HPP_HEXCLAW_OPTIONS


#include "../hexclaw_constants.hpp"
#include "../hexclaw_includes.hpp"
#include "../hexclaw_global.hpp"
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

/// @brief run intro servo movement
void HW_option1_intro();
/// @brief calibrate HSV values
void HW_option2();

/// @brief mode0: run main/default option
void HW_option0();
/// @brief mode 1: track with opencv and display result but don't control/move servo motors
void HW_option3();
/// @brief mode 2: move servo with tracking, don't display in opencv window. just show in terminal
void HW_option4();

void HW_setup_options() {
    hexclaw_cmdArgs.add_func("-h", cmdArgs_info, "--help", "show help message with info on flags");
    hexclaw_cmdArgs.add_func("-i", HW_option1_intro, "--intro", "run only servo intro movement");
    hexclaw_cmdArgs.add_func("-c", HW_option2, "--cal", "calibrate/setup \"Hue Saturation Value\" -values");
    hexclaw_cmdArgs.add_func("-m", HW_option0, "--main", "run main/default version with servo control and opencv based tracking");
    hexclaw_cmdArgs.add_func("-0", HW_option0, "--mode0");
    hexclaw_cmdArgs.add_func("-1", HW_option3, "--mode1", "track and display with opencv but don't send control movements to robot motors");
    hexclaw_cmdArgs.add_func("-2", HW_option4, "--mode2", "track and control servo motors with opencv, but don't create and display opencv images");
    
}


void HW_option1_intro() {
		current_q[0] = -45;
		current_q[1] = 25;
		current_q[2] = -115;
		current_q[3] = -45;
		current_q[4] = -90;
		current_q[5] = 90;
		add_defaults(current_q);
		new_q[0] = 0;
		new_q[1] = 135;
		new_q[2] = -90;
		new_q[3] = 0;
		new_q[4] = -45;
		new_q[5] = 0;
		// printf("running intro...\n");
        //sendToServo(&pca, current_q, new_q, false, 0, 0);
		usleep(1'000'000);
		if(pigpioInitia) {
			gpioWrite(pin_ledRelay, 0);
			gpioWrite(pin_ledRelay, 1);
		}
		usleep(750'000);
		printf("\n- section: \"slow start\"\n");
		sendToServo(&pca, new_q, current_q, false, 2, 10);

		printf("intro finished\n");
		usleep(3'000'000);
		if(pigpioInitia) {
			for(int i=0; i<4; i++) {
				gpioWrite(pin_ledRelay, 0);
				usleep(30'000);
				gpioWrite(pin_ledRelay, 1);
				usleep(30'000);
			}
			usleep(1'500'000);
			gpioWrite(pin_ledRelay, 0);
			usleep(250'000);
			gpioWrite(pin_ledRelay, 1);
			usleep(500'000);
			gpioWrite(pin_ledRelay, 0);
			usleep(100'000);
			gpioWrite(pin_ledRelay, 1);
			usleep(2'000'000);
			gpioWrite(pin_ledRelay, 0);
		}
		usleep(2'000'000);

		if(pigpioInitia) gpioWrite(pin_ledRelay, 1);
		new_q[0] = 45;
		new_q[1] = 0;
		new_q[2] = -45;
		new_q[3] = 89;
		new_q[4] = 89;
		new_q[5] = 0;
		printf("\n- section: \"crash\"\n");
		sendToServo(&pca,new_q,current_q, false);
		usleep(2'000'000);

}
void HW_option2() {

}

void HW_option0() {

}
void HW_option3() {

}
void HW_option4() {

}



#endif