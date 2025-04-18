

#include "hw_options.hpp"


labeledFunction hexclaw_cmdArgs;
createTable cmdArgs_output_table;



void printFuncLabel(std::string functionName) {
	int termDim[2];
	getTermSize(termDim[0], termDim[1]);
	// std::cout << std::endl << std::string(termDim[0], '-')<<std::endl;
	// std::cout << functionName << std::endl;
	// std::cout << std::string(termDim[0], '-')<<std::endl;
	ANSI_mvprint(0, 2, std::string(termDim[0], '-'), true, "abs", "abs", true);
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
    std::string textCell_text="";
    if(serverObj.isClientConnected()) {
        textCell_text = "client: "+serverObj.getClientIPaddr()+","+std::to_string(serverObj.getClientPort());
    }
    else {
        textCell_text = "client: no client connected. init.";
    }
    // SHLEEP((100));

    menu_group__main.driver(1, 1, 5, true, _mainDriver_updateFunc, true);


}
void HW_group__calibrate() {

    menu_group__calibrate.driver(1, 1, 5, true);
}


void HW__config_options() {
    static bool _init = true;
    static size_t maxStrSize = 0;

    maxStrSize = _CONFIG_OPTIONS.getKey(getVal_findString(_CONFIG_OPTIONS.keys(), 0)).length();
    
    // ANSI_mvprint(0, 0, "", true, "abs", "abs", true);
    while(true) {
        for(size_t i=0; i<_CONFIG_OPTIONS.size(); i++) {
            menu__config_options.addOpt(formatNumber(_CONFIG_OPTIONS.getKey(i), maxStrSize+1, 0, "left")+":  "+formatNumber(_CONFIG_OPTIONS[i], 5, 0, "left"), 0, i, -1, static_cast<TUI::TDEF_void__>(nullptr));
        }
        menu__config_options.addOpt("back", 0, 1+_CONFIG_OPTIONS.size(), 27, TUI::DEDICATED__exitDriver);
        pos2d<int> pressed_pos = menu__config_options.driver(1, 1, 5, true);
        if(pressed_pos.inRegion({0, 0}, {0, static_cast<int>(_CONFIG_OPTIONS.size())})) {
            _CONFIG_OPTIONS[pressed_pos.y] = !_CONFIG_OPTIONS[pressed_pos.y];
        }
        if(pressed_pos==pos2d<int>{0, static_cast<int>(1+_CONFIG_OPTIONS.size()) /*1+static_cast<int>(_CONFIG_OPTIONS.size())*/}) {
            break;
        }
    }

    if(_init) _init = false;
}

void HW__init_options() {
    static bool _init = true;
    static size_t maxStrSize = 0;
    // if(_init) {
        maxStrSize = _init_status.getKey(getVal_findString(_init_status.keys(), 0)).length();
    // }
    // menu__init_options.addOpt_nullFunc("maxStrSize: "+formatNumber(maxStrSize), 5, 0, -1);
    // menu__init_options.addOpt_nullFunc("keys: "+formatVector(_init_status.keys()), 5, 1, -1);
    // menu__init_options.addOpt_nullFunc("_sizes: "+formatVector(_tempRef), 5, 2, -1);
    // menu__init_options.addOpt_nullFunc("key idx: "+formatNumber(getVal_findString(_init_status.keys(), 0)), 5, 3, -1);

    while(true) {
        for(size_t i=0; i<_init_status.size(); i++) {
            menu__init_options.addOpt(
                " "+formatNumber(_init_status.getKey(i), maxStrSize+1, 0, "left")+": "+formatNumber(_init_status[i].isInit(), 5, 0, "left"),
                0, i, -1, static_cast<TUI::TDEF_void__>(nullptr)
            );
        }
        menu__init_options.addOpt("back", 0, 1+_init_status.size(), 27, TUI::DEDICATED__exitDriver);

        pos2d<int> pressed_pos = menu__init_options.driver(1, 1, 5, true);
        if(pressed_pos.inRegion({0, 0}, {0, static_cast<int>(_init_status.size())})) {
            /// Calling a init status value
            if(_init_status[pressed_pos[1]].isInit()) {
                /// init status being turned off
                if(_init_status[pressed_pos[1]].call_closing()) {
                    menu__init_options.addOpt("close failed: "+_init_status[pressed_pos[1]].get_callMsg(), 2, pressed_pos[1], -1, static_cast<TUI::TDEF_void__>(nullptr));
                }
            }
            else {
                /// init status being called to turn on
                std::string call_msg = "";
                if(_init_status[pressed_pos[1]].call_init()) {
                    call_msg = "init  failed    : "+_init_status[pressed_pos[1]].get_callMsg();
                }
                else {
                    call_msg = "init  success   : "+_init_status[pressed_pos[1]].get_callMsg();
                }
                menu__init_options.addTextCell(call_msg, 2, pressed_pos[1]);


            }
        }
        if(pressed_pos==pos2d<int>{0, static_cast<int>(1+_init_status.size())}) {
            break;
        }
    }
    if(_init) _init = false;

}


void _mainDriver_updateFunc(TUI::termMenu* ptr_menu) {
    ptr_menu->addTextCell("FPS:"+formatNumber(ptr_menu->FPS, 6, 1), 6, 0);

    _mainDriver_updateFunc__serverObj_clientInfo(ptr_menu);
    _mainDriver_updateFunc__orientObj(ptr_menu);

    ptr_menu->updateTable();
}
void _mainDriver_updateFunc__serverObj_clientInfo(TUI::termMenu* ptr_menu) {
    static std::string prevStr="";
    std::string textCell_text=formatNumber("serverObj",10,0,"left")+": ";
    if(!serverObj.isRunning()) {
        textCell_text += "not running.";
    }
    else {
        if(serverObj.isClientConnected()) {
            textCell_text += serverObj.getClientIPaddr()+","+std::to_string(serverObj.getClientPort());
        }
        else {
            textCell_text += "no client found.";
        }
    }
    
    ptr_menu->addTextCell(textCell_text, 6, 1);
    if(prevStr!=textCell_text) {
        // ptr_menu->updateTable();
        prevStr = textCell_text;
    }
}
void _mainDriver_updateFunc__orientObj(TUI::termMenu* ptr_menu) {
    static std::string prevStr="";
    std::string textCell_text=formatNumber("orientObj",10,0,"left")+": ";
    if(!_init_status.get("orientObj").isInit()) {
        textCell_text += "not running.";
    }
    else {
        try {
            orientObj.update(false);
            textCell_text += std::string(formatContainer1(orientObj.accel, 3, 5, 2));
        }
        catch(std::exception &e) {
            textCell_text += e.what();
        }
    }

    ptr_menu->addTextCell(textCell_text, 6, 2);
    if(prevStr!=textCell_text) {
        // ptr_menu->updateTable();
        prevStr = textCell_text;
    }

}
