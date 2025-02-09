

#include "hw_options.hpp"


labeledFunction hexclaw_cmdArgs;
createTable cmdArgs_output_table;


void printFuncLabel(std::string functionName) {
	int termDim[2];
	getTermSize(termDim[0], termDim[1]);
	// std::cout << std::endl << std::string(termDim[0], '-')<<std::endl;
	// std::cout << functionName << std::endl;
	// std::cout << std::string(termDim[0], '-')<<std::endl;
	ANSI_mvprint(0, 0, std::string(termDim[0], '-'), true, "abs", "rel", true);
	ANSI_mvprint(0, 0, functionName, true, "abs", "rel");
	ANSI_mvprint(0, 0, std::string(termDim[0], '-'), true, "abs", "rel");
}

void cmdArgs_info() {

    std::vector<size_t> num_names;
    // std::vector<size_t> names_sum_lengths;
    for(size_t i=0; i<hexclaw_cmdArgs.getDescriptions().size(); i++) {
        // names_sum_lengths.push_back(get_sumStrVec_len(hexclaw_cmdArgs.getNames()[i]));
        num_names.push_back(hexclaw_cmdArgs.getNames()[i].size());
    }
    // size_t maxNamesWidth = findVal<size_t>(names_sum_lengths, 0);
    size_t maxNumNames = findVal(num_names, 0);
    cmdArgs_output_table = createTable(maxNumNames+1, hexclaw_cmdArgs.getDescriptions().size());

    for(size_t i=0; i<hexclaw_cmdArgs.getDescriptions().size(); i++) {
        std::vector<std::string> _currNames = hexclaw_cmdArgs.getNames().at(i);
        for(size_t _x=0; _x<_currNames.size(); _x++) {
            cmdArgs_output_table.insertText(_currNames.at(_x), _x, i);
        }

        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getDescriptions()[i], maxNumNames, i);
    }

    cmdArgs_output_table.strExport("\n",false,"\n","  ");
    std::cout << "\t"<<cmdArgs_output_table.exportStr << std::endl;
	hardExit = true;
}

void HW_setup_options() {
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-h", "--help"}, cmdArgs_info, "Show help message with info on flags");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-i", "--intro"}, HW_option1_intro, "Run only servo intro movement");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-c", "--cal"}, HW_option2, "Calibrate/setup \"Hue Saturation Value\" -values");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-0", "-m", "--main"}, HW_option0, "Run main/default version with servo control, opencv based tracking");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-3"}, HW_option3, "Track and display with opencv but don't send control movements to robot motors");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-4"}, HW_option4, "Calibrate servo motors by reading error rate and solving reverse correction function coefficients");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-5"}, HW_option5_orient, "Only read and send end-effector orientation from nodemcu device to hexclaw robot-arm");
	hexclaw_cmdArgs.add_func(std::vector<std::string>{"-6", "-t", "--terminal"}, HW_option6_terminal, "Control robot with terminal position inputs");

}


void HW_group__main() {

    menu_group__main.driver(1, 1, 5, true);
}

void HW_group__calibrate() {

    menu_group__calibrate.driver(1, 1,  5, true);

}
