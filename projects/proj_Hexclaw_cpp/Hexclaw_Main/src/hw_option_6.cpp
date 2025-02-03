/**
 * @file hw_option_6.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "hw_options.hpp"

bool opt6_hardExit = false;
bool opt6_localExit = false;



void HW_option6_control_panel();
void HW_option6_rawTerminal();
void HW_option6_settings();
void HW_option6_runFromFile();


void HW_option6_terminal() {

	int winDim[2] = {0, 0};
	int cursorPos[2] = {0, 0};
	
	getTermSize(winDim[0], winDim[1]);

    /**
     * - Give xyz pos {pathless, linear path, custom path}
     * - Give orient {locked pos}
     * - 
     * - Run from file
     * - Settings
    */
    

    opt6_startMenu.addOpt("[1]   Control Panel", 0, 0, '1', HW_option6_control_panel);
    opt6_startMenu.addOpt("[2]   Raw input    ", 0, 1, '2', HW_option6_rawTerminal);
    opt6_startMenu.addOpt("[3]   Run file     ", 0, 2, '3', HW_option6_runFromFile);
    opt6_startMenu.addOpt("[s]   Settings     ", 0, 4, 's', HW_option6_settings);
    opt6_startMenu.addOpt("[esc] Back         ", 0, 5, 27, bool_true, &opt6_startMenu.exitDriver);
    opt6_startMenu.addOpt("[e]   Exit         ", 0, 6, 'e', exitFrom_lvl2, &opt6_startMenu.exitDriver);


    opt6_startMenu.driver(1, 1, 0, false);
}


void HW_option6_exit_control_panel(bool* boolVar) { 
    opt6_startMenu.exitDriver = true;
    opt6_control_panel.exitDriver = true;
    (*boolVar) = true;
}
void HW_option6_control_panel() {
    int digits_integers = 4; // number of whole numbers
    int digits_decimals = 3;

    int termLoc_title_bar[2]    = {1, 1};       //rows: 3
    int termLoc_control_panel[2]= {1, 3};       //rows: 7
    int termLoc_program_out[2]  = {1, 4+2+7};   //rows: 1
    int termLoc_output_angles[2]= {1, 4+2+7+1}; //rows: 5

    int key_back[2] = {1, 5};
    int key_enter[2]= {5, 5};

    std::string outMsg="", prev_outMsg="";

    /**
     * Table to display output, error codes, info e.t.c.
     *  _ _ _ _ _ _ _ _ _
     *  _ 0 1 2 3 4 5 _ _ (q raw kinematic angles)
     *  _ 0 1 2 3 4 5 _ _ (q servo motor rotations)
     *  _ _ _ _ _ _ _ _ _
     *  _ _ _ _ _ _ _ _ _
    */
    createTable infoTable(9, 5);


    bool exitLoop = false;

    opt6_control_panel.addOpt("x:"+std::string(1+digits_integers+digits_decimals,' '),1,1,-1,termMenu_nullFunc_void__);
    opt6_control_panel.addOpt("y:"+std::string(1+digits_integers+digits_decimals,' '),3,1,-1,termMenu_nullFunc_void__);
    opt6_control_panel.addOpt("z:"+std::string(1+digits_integers+digits_decimals,' '),5,1,-1,termMenu_nullFunc_void__);

    opt6_control_panel.addOpt("a:"+std::string(1+digits_integers+digits_decimals,' '),1,3,-1,termMenu_nullFunc_void__);
    opt6_control_panel.addOpt("B:"+std::string(1+digits_integers+digits_decimals,' '),3,3,-1,termMenu_nullFunc_void__);
    opt6_control_panel.addOpt("Y:"+std::string(1+digits_integers+digits_decimals,' '),5,3,-1,termMenu_nullFunc_void__);

    opt6_control_panel.addOpt("back", key_back[0],key_back[1],27, termMenu_nullFunc_void__);
    opt6_control_panel.addOpt("enter",key_enter[0],key_enter[1],-1, termMenu_nullFunc_void__);


    const int* pressedKey;
    std::string keyInput_str, printStr;
    float keyInput_float = 0;

    float PP_pos[3] = {0, 0, 0};
    float PP_tilt[3] = {0, 0, 0};
    float temp_q[6] = {0,0,0,0,0,0};
    /// @brief container of boolean to check if a value has been added/set to an input box
    bool init_PP_init[2][3] = {{false,false,false},{true,true,true}};
    int pressedTermPos[2] = {0, 0};

    /**
     * Outer limits of the allowed values input.
     * coords: work envelope {min:max}
     * orient: -90:90 {min:max}
    */
    int inpLim[2][2] = {
        {-static_cast<int>(array_sum(HW_KINEMATICS::arm_link,6)), static_cast<int>(array_sum(HW_KINEMATICS::arm_link,6))},
        {-90, 90}
    };

    std::cout << opt6_control_panel.ansi_code << "2J";
    std::cout.flush();

    ANSI_mvprint(termLoc_title_bar[0], termLoc_title_bar[1], "Control Panel");
    ANSI_mvprint(termLoc_title_bar[0], termLoc_title_bar[1]+1, "  input three of each parameter type (coordinate, tilt)");
    ANSI_mvprint(
        termLoc_title_bar[0], termLoc_title_bar[1]+2, 
        "  range: coord{"+std::to_string(inpLim[0][0])+":"+std::to_string(inpLim[0][1])+"}"+" orient{"+std::to_string(inpLim[1][0])+":"+std::to_string(inpLim[1][1])+"}"
    );



    float tempLim=0;

    bool tabCalled = false;
    std::vector<int> tabCallVec{32, KEY_ENTER};
    while(!exitLoop) {
        if(tabCalled) {
            tabCalled = false;
            opt6_control_panel.init_driverCallKeys_clear();
            opt6_control_panel.moveCurrCell(1,0);
            opt6_control_panel.init_driverCallKeys_add(tabCallVec);
        }
        else {
            opt6_control_panel.init_driverCallKeys_clear();
        }
        pressedKey = opt6_control_panel.driver(termLoc_control_panel[0],termLoc_control_panel[1],0,false,termMenu_nullFunc_void__,false);
        if(tabCalled) {
            tabCalled = false;
            opt6_control_panel.init_driverCallKeys_clear();
        }
        if(pressedKey[0]==key_back[0] && pressedKey[1]==key_back[1]) {
            endwin();
            break;
        }
        if(pressedKey[0]%2!=0 && pressedKey[0]<=5 && pressedKey[1]%2!=0 && pressedKey[1]<=3) {
            pressedTermPos[0] = opt6_control_panel.getCellPos_x(pressedKey[0],pressedKey[1])+termLoc_control_panel[0]+2;
            pressedTermPos[1] = opt6_control_panel.getCellPos_y(pressedKey[0],pressedKey[1])+termLoc_control_panel[1];
            
            keyInput_str = opt6_control_panel.termInput(
                1,
                pressedTermPos[0],
                pressedTermPos[1],
                1+digits_integers+digits_decimals,1,"left","37","40",
                opt6_control_panel.getCellName(pressedKey[0],pressedKey[1]).substr(0,2),
                &tabCalled
            );
            if(isNumber(keyInput_str)) {
                keyInput_float = stof(keyInput_str);
                if(
                    (pressedKey[1]==1 && keyInput_float>=inpLim[0][0] && keyInput_float<=inpLim[0][1]) ||
                    (pressedKey[1]==3 && keyInput_float>=inpLim[1][0] && keyInput_float<=inpLim[1][1])
                ){
                    printStr = formatNumber(keyInput_float,digits_integers,digits_decimals);
                    opt6_control_panel.rename_opt(
                        pressedKey[0],pressedKey[1],
                        opt6_control_panel.getCellName(pressedKey[0],pressedKey[1]).substr(0,2) + 
                        printStr
                    );
                    if(pressedKey[1]==1) {
                        if(pressedKey[0]==1) PP_pos[0] = keyInput_float;
                        if(pressedKey[0]==3) PP_pos[1] = keyInput_float;
                        if(pressedKey[0]==5) PP_pos[2] = keyInput_float;
                    }
                    else if(pressedKey[1]==3) {
                        if(pressedKey[0]==1) PP_tilt[0] = keyInput_float;
                        if(pressedKey[0]==3) PP_tilt[1] = keyInput_float;
                        if(pressedKey[0]==5) PP_tilt[2] = keyInput_float;
                    }
                    int tempIdx[2] = { // 1 3 5; 1 3
                        static_cast<int>(floor(static_cast<float>(pressedKey[0]-1)/2)),
                        (pressedKey[1]-1)/2
                    };
                    if(
                        !init_PP_init[tempIdx[1]][tempIdx[0]]
                    ) init_PP_init[tempIdx[1]][tempIdx[0]]=true;
                }
                else {
                    if(
                        (pressedKey[1]==1 && keyInput_float<inpLim[0][0]) ||
                        (pressedKey[1]==3 && keyInput_float<inpLim[1][0])
                    ) outMsg="input number too small";
                    else if(
                        (pressedKey[1]==1 && keyInput_float>inpLim[0][0]) ||
                        (pressedKey[1]==3 && keyInput_float>inpLim[1][0])
                    ) outMsg="input number too large";
                }
            }
            else outMsg="input is not a number";
        }
        if(pressedKey[0]==key_enter[0] && pressedKey[1]==key_enter[1]) { // 'enter' key pressed
            bool boolRow[2] = {array_bool_AND(init_PP_init[0],3), array_bool_AND(init_PP_init[1],3)};
            if(boolRow[0] && boolRow[1]) {
                //true true: new pos, new tilt
                if(HW_KINEMATICS::getAngles(
                    new_q, PP_pos,
                    toRadians(PP_tilt[0]),
                    toRadians(PP_tilt[1]),
                    toRadians(PP_tilt[2]),
                    1
                )) {
                    if(_init_status.get("pca").isInit()) {
                        sendToServo(&pca, new_q, current_q,false,2,0.5);
                    }
                    outMsg = "Success: sent new rotations to servo motors";

                }
                else if(HW_KINEMATICS::findValidOrient(PP_pos, PP_tilt, PP_tilt, new_q)) {
                    for(int qu=0; qu<3; qu++) {
                        printStr = formatNumber(PP_tilt[qu],digits_integers,digits_decimals);
                        opt6_control_panel.rename_opt(
                            qu*2+1,3,
                            opt6_control_panel.getCellName(qu*2+1,3).substr(0,2) + 
                            printStr
                        );

                    }
                    if(_init_status.get("pca").isInit()) {
                        //pos reachable with new orient
                        sendToServo(&pca, new_q, current_q,false,2,0,5);
                    }
                    outMsg = "Deviant: given orient gave errors -> found new orients";
                }
                else {
                    //pos not reachable
                    outMsg = "ERROR: no possible orientation for given PP pos";
                }
            }
            else { outMsg="missing parameters"; }

            prev_outMsg = outMsg;

            infoTable.insertText("inv_kinem. :", 0, 1); // row[1] label
            infoTable.insertText("q_servo    :", 0, 2); // row[2] label
            for(int q=0; q<6; q++) {
                infoTable.insertText("q["+std::to_string(q)+"]", q+1, 0); // column label
                infoTable.insertNum(HW_KINEMATICS::solved_q[q], q+1, 1, 2);
                infoTable.insertNum(new_q[q], q+1, 2, 2);
            }
            infoTable.strExport("",false,"\n","  ");
            termLoc_output_angles[1] = opt6_control_panel.getMenu_height() + termLoc_control_panel[1] + 4;
            ansiPrint(
                infoTable.exportStr,
                static_cast<int>(termLoc_output_angles[0]),
                static_cast<int>(termLoc_output_angles[1])
            );
        }
        termLoc_program_out[1] = opt6_control_panel.getMenu_height() + termLoc_control_panel[1] + 2;
        ANSI_mvprint(termLoc_program_out[0], termLoc_program_out[1], ">> "+std::string(prev_outMsg.length(), ' '));
        ANSI_mvprint(termLoc_program_out[0], termLoc_program_out[1], ">> "+outMsg);
    }
}
void HW_option6_rawTerminal() {

}
void HW_option6_settings() {

}
void HW_option6_runFromFile() {

}
