

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

    menu_group__calibrate.driver(1, 1, 5, true);

}


void HW__config_options() {
    static bool _init = true;
    static size_t maxStrSize = 0;
    if(_init) {
        maxStrSize = _CONFIG_OPTIONS.getKey(getVal_findString(_CONFIG_OPTIONS.keys(), 0)).length();
    }
    TUI::n_print = true;
    ANSI_mvprint(0, 0, "", true, "abs", "abs", true);
    while(true) {
        for(size_t i=0; i<_CONFIG_OPTIONS.size(); i++) {
            menu__config_options.addOpt(formatNumber(_CONFIG_OPTIONS.getKey(i), maxStrSize+1, 0, "left")+":  "+formatNumber(_CONFIG_OPTIONS[i], 5, 0, "left"), 0, i, -1, static_cast<TUI::TDEF_void__>(nullptr));
        }
        menu__config_options.addOpt("exit", 0, 1+_CONFIG_OPTIONS.size(), 27, TUI::DEDICATED__exitDriver);
    
        pos2d<int> pressed_pos = menu__config_options.driver(1, 1, 1, true);

        if(pressed_pos.inRegion({0, 0}, {0, static_cast<int>(_CONFIG_OPTIONS.size())})) {
            _CONFIG_OPTIONS[pressed_pos.y] = !_CONFIG_OPTIONS[pressed_pos.y];
        }
        if(pressed_pos==pos2d<int>{0, static_cast<int>(1+_CONFIG_OPTIONS.size()) /*1+static_cast<int>(_CONFIG_OPTIONS.size())*/}) {
            break;
        }
    }

    if(_init) _init = false;
}
