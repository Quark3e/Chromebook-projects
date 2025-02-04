

#include "hw_options.hpp"


labeledFunction hexclaw_cmdArgs;
createTable cmdArgs_output_table;


void printFuncLabel(std::string functionName) {
	int termDim[2];
	getTermSize(termDim[0], termDim[1]);
	// std::cout << std::endl << std::string(termDim[0], '-')<<std::endl;
	// std::cout << functionName << std::endl;
	// std::cout << std::string(termDim[0], '-')<<std::endl;
	ANSI_mvprint(0, 0, std::string(termDim[0], '-'));
	ANSI_mvprint(0, 0, functionName, true, "abs", "rel");
	ANSI_mvprint(0, 0, std::string(termDim[0], '-'), true, "abs", "rel");
}

void cmdArgs_info() {
    cmdArgs_output_table = createTable(3, hexclaw_cmdArgs.getDescriptions().size());

    for(size_t i=0; i<hexclaw_cmdArgs.getDescriptions().size(); i++) {
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getNames()[i][0], 0, i);
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getNames()[i][1], 1, i);
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getDescriptions()[i], 2, i);
    }

    cmdArgs_output_table.strExport("\n",false,"\n","  ");
    std::cout << "\t"<<cmdArgs_output_table.exportStr << std::endl;
	hardExit = true;
}

void HW_setup_options() {
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-h", "--help"}, cmdArgs_info, "show help message with info on flags");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-i", "--intro"}, HW_option1_intro, "run only servo intro movement");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-c", "--cal"}, HW_option2, "calibrate/setup \"Hue Saturation Value\" -values");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-0", "-m", "--main"}, HW_option0, "run main/default version with servo control, opencv based tracking");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-3"}, HW_option3, "track and display with opencv but don't send control movements to robot motors");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-4"}, HW_option4, "calibrate servo motors by reading error rate and solving reverse correction function coefficients");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-5"}, HW_option5_orient, "only read and send end-effector orientation from nodemcu device to hexclaw robot-arm");
	hexclaw_cmdArgs.add_func(std::vector<std::string>{"-6", "-t", "--terminal"}, HW_option6_terminal, "control robot with terminal position inputs");

}
