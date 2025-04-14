
#include "terminalMenu.hpp"

// #include <conio.h>
#include <stdio.h>


TUI::optItem::optItem(std::string _name, int _x, int _y, int _key
): name(_name), x(_x), y(_y), key(_key), _funcType_id(0), func_void__(nullptr) {

}
TUI::optItem::optItem(std::string _name, int _x, int _y, int _key, TDEF_void__     _func
): name(_name), x(_x), y(_y), key(_key), _funcType_id(0), func_void__(_func) {

}
TUI::optItem::optItem(std::string _name, int _x, int _y, int _key, TDEF_void_pBool _func, bool *_param
): name(_name), x(_x), y(_y), key(_key), _funcType_id(1), func_void_pBool(_func), param_pBool(_param) {

}
TUI::optItem::optItem(std::string _name, int _x, int _y, int _key, TDEF_void_pInt  _func, int  *_param
): name(_name), x(_x), y(_y), key(_key), _funcType_id(2), func_void_pInt(_func), param_pInt(_param) {

}
TUI::optItem::optItem(std::string _name, int _x, int _y, int _key, TDEF_void_pChar _func, char *_param
): name(_name), x(_x), y(_y), key(_key), _funcType_id(3), func_void_pChar(_func), param_pChar(_param) {

}
TUI::optItem::optItem(std::string _name, int _x, int _y, int _key, TDEF_void_pStr  _func, std::string *_param
): name(_name), x(_x), y(_y), key(_key), _funcType_id(4), func_void_pStr(_func), param_pStr(_param) {

}



void TUI::termMenu::extend_containers(int xAdd, int yAdd) {
    if(xAdd<1 && yAdd<1) return;

    if(option_key.size()==0) {
        menuTable = createTable(xAdd, yAdd);
    }
    else {
        menuTable.add_col(xAdd);
        menuTable.add_row(yAdd);
    }

    int xSize = menuTable.table.at(0).size();
    for(int yDiff=0; yDiff<yAdd; yDiff++) {
        option_key.push_back(std::vector<int>(xSize, -1));
        optFunc_identifier.push_back(std::vector<int>(xSize, -1));

        optFunc_id0.push_back(std::vector<TDEF_void__>(xSize, termMenu_nullFunc_void__));
        optFunc_id1_.push_back(std::vector<TDEF_void_pBool>(xSize, termMenu_nullFunc_void_pBool));
        optFunc_id2_.push_back(std::vector<TDEF_void_pInt>(xSize, termMenu_nullFunc_void_pInt));
        optFunc_id3_.push_back(std::vector<TDEF_void_pChar>(xSize, termMenu_nullFunc_void_pChar));
        optFunc_id4_.push_back(std::vector<TDEF_void_pStr>(xSize, termMenu_nullFunc_void_pStr));

        optFunc_id1_p0.push_back(std::vector<bool*>(xSize, &termMenu_nullParam_bool));
        optFunc_id2_p0.push_back(std::vector<int*>(xSize, &termMenu_nullParam_int));
        optFunc_id3_p0.push_back(std::vector<char*>(xSize, &termMenu_nullParam_char));
        optFunc_id4_p0.push_back(std::vector<std::string*>(xSize, &termMenu_nullParam_string));
    }
    for(int y=0; y<menuTable.table.size(); y++) {
        for(int xDiff=0; xDiff<xAdd; xDiff++) {
            option_key.at(y).push_back(-1);
            optFunc_identifier.at(y).push_back(-1);

            optFunc_id0.at(y).push_back(termMenu_nullFunc_void__);
            optFunc_id1_.at(y).push_back(termMenu_nullFunc_void_pBool);
            optFunc_id2_.at(y).push_back(termMenu_nullFunc_void_pInt);
            optFunc_id3_.at(y).push_back(termMenu_nullFunc_void_pChar);
            optFunc_id4_.at(y).push_back(termMenu_nullFunc_void_pStr);

            optFunc_id1_p0.at(y).push_back(&termMenu_nullParam_bool);
            optFunc_id2_p0.at(y).push_back(&termMenu_nullParam_int);
            optFunc_id3_p0.at(y).push_back(&termMenu_nullParam_char);
            optFunc_id4_p0.at(y).push_back(&termMenu_nullParam_string);
        }
    }
    menuTable.checkMaxLen();
}
int TUI::termMenu::container_insertKey(int keyPress, int x_column, int y_row) {
    if(keyPress==-1) {
        option_key.at(y_row).at(x_column) = -1;
    }
    else {
        bool toBreak=false;
        for(int y=0; y<option_key.size(); y++) {
            for(int x=0; x<option_key.at(y).size(); x++) {
                if(x==x_column && y==y_row) continue;
                if(option_key.at(y).at(x)==keyPress) {
                    toBreak = true;
                    break;
                }
            }
            if(toBreak) break;
        }
        if(toBreak) option_key.at(y_row).at(x_column) = -1;
        else option_key.at(y_row).at(x_column) = keyPress;
        // return -1;
    }
    return 0;
}
int TUI::termMenu::customCall_argLess(int pos_x, int pos_y) {
    switch (optFunc_identifier.at(pos_y).at(pos_x)) {
        case 0: if(optFunc_id0.at(pos_y).at(pos_x)) optFunc_id0.at(pos_y).at(pos_x)(); break;
        case 1: if(optFunc_id1_.at(pos_y).at(pos_x)) optFunc_id1_.at(pos_y).at(pos_x)(optFunc_id1_p0.at(pos_y).at(pos_x)); break;
        case 2: if(optFunc_id2_.at(pos_y).at(pos_x)) optFunc_id2_.at(pos_y).at(pos_x)(optFunc_id2_p0.at(pos_y).at(pos_x)); break;
        case 3: if(optFunc_id3_.at(pos_y).at(pos_x)) optFunc_id3_.at(pos_y).at(pos_x)(optFunc_id3_p0.at(pos_y).at(pos_x)); break;
        case 4: if(optFunc_id4_.at(pos_y).at(pos_x)) optFunc_id4_.at(pos_y).at(pos_x)(optFunc_id4_p0.at(pos_y).at(pos_x)); break;
        default:
            return -1;
    }

    return 0;
}


TUI::termMenu::termMenu(
    int menuColumns,
    int menuRows,
    bool callFromDriver
) {
    if(menuColumns<1 || menuRows<1) {
        throw std::invalid_argument("ERROR: termMenu init: menu{Columns/Rows} can't be smaller than 1");
    }
    callFunc = callFromDriver;

    
    extend_containers(
        menuColumns,
        menuRows
    );

    classInit = true;
}
TUI::termMenu::termMenu(
    std::initializer_list<TUI::optItem> _items,
    bool callFromDriver
) {
    for(auto itr=_items.begin(); itr!=_items.end(); ++itr) {
        switch (itr->_funcType_id) {
            case 0: this->addOpt(itr->name, itr->x, itr->y, itr->key, itr->func_void__); break;
            case 1: this->addOpt(itr->name, itr->x, itr->y, itr->key, itr->func_void_pBool, itr->param_pBool);break;
            case 2: this->addOpt(itr->name, itr->x, itr->y, itr->key, itr->func_void_pInt,  itr->param_pInt); break;
            case 3: this->addOpt(itr->name, itr->x, itr->y, itr->key, itr->func_void_pChar, itr->param_pChar);break;
            case 4: this->addOpt(itr->name, itr->x, itr->y, itr->key, itr->func_void_pStr,  itr->param_pStr); break;
        default:
            break;
        }
    }

    classInit = true;
    callFunc = callFromDriver;

}
TUI::termMenu::termMenu(
    std::initializer_list<TUI::optItem> _items,
    int menuColumns,
    int menuRows,
    bool callFromDriver
) {
    if(menuColumns<1 || menuRows<1) {
        throw std::invalid_argument("ERROR: termMenu init: menu{Columns/Rows} can't be smaller than 1");
    }
    callFunc = callFromDriver;

    
    extend_containers(
        menuColumns,
        menuRows
    );
    
    for(auto itr=_items.begin(); itr!=_items.end(); ++itr) {
        switch (itr->_funcType_id) {
            case 0: this->addOpt(itr->name, itr->x, itr->y, itr->key, itr->func_void__); break;
            case 1: this->addOpt(itr->name, itr->x, itr->y, itr->key, itr->func_void_pBool, itr->param_pBool);break;
            case 2: this->addOpt(itr->name, itr->x, itr->y, itr->key, itr->func_void_pInt,  itr->param_pInt); break;
            case 3: this->addOpt(itr->name, itr->x, itr->y, itr->key, itr->func_void_pChar, itr->param_pChar);break;
            case 4: this->addOpt(itr->name, itr->x, itr->y, itr->key, itr->func_void_pStr,  itr->param_pStr); break;
        default:
            break;
        }
    }

    classInit = true;
}

TUI::termMenu::~termMenu() {
#ifndef _WIN32
    if(_ncurses_windowOpen) endwin();
#else
    std::cout << ansi_code + "2J" << std::endl;
#endif
}

void TUI::DEDICATED__exitDriver(bool* _driverExit) {
    assert(_driverExit && "ERROR: incorrect pointer sent to TUI::DEDICATED__exitDriver function");
    *_driverExit = true;
}

int TUI::termMenu::setDisplayDim(
    int c0_x,
    int c0_y,
    int c1_x,
    int c1_y,
    int cursMode
) {
    if(cursMode<-1 || cursMode >2) return -1;


    if(c0_x==-1 || c0_y==-1 || c1_x==-1 || c1_y==-1) {
        cursorMode = -1;
        display_dim[0][0] = -1;
        display_dim[0][1] = -1;
        display_dim[1][0] = -1;
        display_dim[1][1] = -1;

        // display_dim[0][0] = menuTable.getCellX(0, 0);
        // display_dim[0][1] = menuTable.getCellY(0, 0);
        // display_dim[1][0] = menuTable.getCellX(static_cast<int>((menuTable.table.size()>0? menuTable.table.at(0).size() : 0) -1 ), static_cast<int>(menuTable.table.size())-1);
        // display_dim[1][1] = menuTable.getCellY(static_cast<int>((menuTable.table.size()>0? menuTable.table.at(0).size() : 0) -1 ), static_cast<int>(menuTable.table.size())-1);
    }
    else {
        cursorMode = cursMode;
        display_dim[0][0] = c0_x;
        display_dim[0][1] = c0_y;
        display_dim[1][0] = c1_x;
        display_dim[1][1] = c1_y;
    }

    return 0;
}


int TUI::termMenu::setButtonWidth(int width) {
    if(width-menuTable.getColTotMax()<0) return -1;

    button_totWidth = width;
    // menuTable.setColPadding(std::vector<int>(menuTable.table.at(0).size(), width));

    return 0;
}

int TUI::termMenu::addOpt_nullFunc(std::string optName, int x_column, int y_row, int keyPress
) {

    extend_containers(
        x_column- (static_cast<int>((menuTable.table.size()>0? menuTable.table.at(0).size() : 0) -1 )),
        y_row   - (static_cast<int>(menuTable.table.size())-1)
    );
    container_insertKey(keyPress, x_column, y_row);
    menuTable.insertText(optName, x_column, y_row);

    optFunc_identifier.at(y_row).at(x_column) = 0;
    optFunc_id0.at(y_row).at(x_column) = nullptr;

    return 0;
}
int TUI::termMenu::addOpt(std::string optName, int x_column, int y_row, int keyPress, TDEF_void__ optFunc
) {

    extend_containers(
        x_column-(static_cast<int>((menuTable.table.size()>0? menuTable.table.at(0).size() : 0) -1 )),
        y_row-(static_cast<int>(menuTable.table.size())-1)
    );
    container_insertKey(keyPress, x_column, y_row);
    menuTable.insertText(optName, x_column, y_row);


    optFunc_identifier.at(y_row).at(x_column) = 0;
    optFunc_id0.at(y_row).at(x_column) = optFunc;

    return 0;
}
int TUI::termMenu::addOpt(std::string optName, int x_column, int y_row, int keyPress, TDEF_void_pBool optFunc, bool *param0
) {

    extend_containers(
        x_column-(static_cast<int>((menuTable.table.size()>0? menuTable.table.at(0).size() : 0) -1 )),
        y_row-(static_cast<int>(menuTable.table.size())-1)
    );
    container_insertKey(keyPress, x_column, y_row);
    menuTable.insertText(optName, x_column, y_row);


    optFunc_identifier.at(y_row).at(x_column) = 1;
    optFunc_id1_.at(y_row).at(x_column) = optFunc;
    if(optFunc==TUI::DEDICATED__exitDriver) {
        optFunc_id1_p0.at(y_row).at(x_column) = &this->exitDriver;
    }
    else {
        optFunc_id1_p0.at(y_row).at(x_column) = param0;
    }

    return 0;
}
int TUI::termMenu::addOpt(std::string optName, int x_column, int y_row, int keyPress, TDEF_void_pInt  optFunc, int  *param0
) {

    extend_containers(
        x_column-(static_cast<int>((menuTable.table.size()>0? menuTable.table.at(0).size() : 0) -1 )),
        y_row-(static_cast<int>(menuTable.table.size())-1)
    );
    container_insertKey(keyPress, x_column, y_row);
    menuTable.insertText(optName, x_column, y_row);


    optFunc_identifier.at(y_row).at(x_column) = 2;
    optFunc_id2_.at(y_row).at(x_column) = optFunc;
    optFunc_id2_p0.at(y_row).at(x_column) = param0;

    return 0;
}
int TUI::termMenu::addOpt(std::string optName, int x_column, int y_row, int keyPress, TDEF_void_pChar optFunc, char* param0
) {

    extend_containers(
        x_column-(static_cast<int>((menuTable.table.size()>0? menuTable.table.at(0).size() : 0) -1 )),
        y_row-(static_cast<int>(menuTable.table.size())-1)
    );
    container_insertKey(keyPress, x_column, y_row);
    menuTable.insertText(optName, x_column, y_row);


    optFunc_identifier.at(y_row).at(x_column) = 3;
    optFunc_id3_.at(y_row).at(x_column) = optFunc;
    optFunc_id3_p0.at(y_row).at(x_column) = param0;

    return 0;
}
int TUI::termMenu::addOpt(std::string optName, int x_column, int y_row, int keyPress, TDEF_void_pStr  optFunc, std::string* param0
) {

    extend_containers(
        x_column-(static_cast<int>((menuTable.table.size()>0? menuTable.table.at(0).size() : 0) -1 )),
        y_row-(static_cast<int>(menuTable.table.size())-1)
    );
    container_insertKey(keyPress, x_column, y_row);
    menuTable.insertText(optName, x_column, y_row);


    optFunc_identifier.at(y_row).at(x_column) = 4;
    optFunc_id4_.at(y_row).at(x_column) = optFunc;
    optFunc_id4_p0.at(y_row).at(x_column) = param0;

    return 0;
}

int TUI::termMenu::addTextCell(std::string cellText, int x_column, int y_row, bool convertCellType
) {
    if(
        !convertCellType &&
        y_row < menuTable.table.size() &&
        x_column < menuTable.table.at(0).size() &&
        (option_key[y_row][x_column]!=CELLOPTSPEC_TEXT && menuTable.table.at(y_row).at(x_column) != "")
    ) {
        throw std::runtime_error("addToCell: convertCellType==false && [y][x]!=\"\"");
    }
    
    int ext_x = x_column-(static_cast<int>((menuTable.table.size()>0? menuTable.table.at(0).size() : 0) - 1));
    int ext_y = y_row-(static_cast<int>(menuTable.table.size())-1);
    
    extend_containers(
        ext_x, ext_y
    );
    container_insertKey(CELLOPTSPEC_TEXT, x_column, y_row);
    menuTable.insertText(cellText, x_column, y_row);
    // if(x_column==5 && y_row==0) std::cout << " [addTextCell called:"<<cellText<<"] "; std::cout.flush();

    return 0;
}

int TUI::termMenu::rename_opt(
    int x_column,
    int y_row,
    std::string new_optName
) {
    if(y_row>=menuTable.table.size() || x_column>=menuTable.table[0].size()) return 1;

    menuTable.insertText(new_optName, x_column, y_row);
    return 0;
}

pos2d<int> TUI::termMenu::driver(
    int pos_x,
    int pos_y,
    int msDelay,
    bool init_clear,
    TDEF_void_pTermMenu loopFunc,
    bool iterClear,
    std::string highlight_textColour,
    std::string highlight_bgColour,
    std::string bg_textColour,
    std::string bg_bgColour
) {
    #ifdef DEBUG__PRINT
    debug_checkPointStr = "0";
    #endif
    if(menuTable.table.size()==0) {
        throw std::logic_error("menuTable.table is empty. No cell's are made.");
    }
    if(pos_x==0) pos_x = 1;
    if(pos_y==0) pos_y = 1;
    #ifdef DEBUG__PRINT
    debug_checkPointStr  = "1";
    #endif
    termLoc[0] = pos_x;
    termLoc[1] = pos_y;

    getTermSize(termSize[0], termSize[1]);
    #ifdef DEBUG__PRINT
    debug_checkPointStr  = "2";
    #endif
    // static int pressed_option[2] = {0, 0};
    pos2d<int> pressed_option(0, 0);
    bool loopInit = false; //boolean for whether the driver loop has ran a full iteration already
    int loopInit_count = 0;
    #ifdef DEBUG__PRINT
    debug_checkPointStr  = "3";
    #endif
    exitDriver = false;
    menuTable.strExport("\n", false, "\n", " ");
    // createTable displayTable = menuTable;
    #ifdef DEBUG__PRINT
    debug_checkPointStr  = "4";
    #endif
    if(display_dim[1][0]-display_dim[0][0]>menuTable.table[0].size()-1) {
        display_dim[1][0] = display_dim[0][0] + menuTable.table[0].size()-1;
    }
    if(display_dim[1][1]-display_dim[0][1]>static_cast<int>(menuTable.table.size())-1) {
        display_dim[1][1] = display_dim[0][1] + static_cast<int>(menuTable.table.size())-1;
    }
    #ifdef DEBUG__PRINT
    debug_checkPointStr  = "5";
    #endif
    if(cursorMode==-1) {
        display_dim[0][0] = 0;
        display_dim[0][1] = 0;
        display_dim[1][0] = menuTable.table[0].size()-1;
        display_dim[1][1] = static_cast<int>(menuTable.table.size())-1;
    }
    if(menuTable.getCellX(menuTable.table[0].size()-1,static_cast<int>(menuTable.table.size())-1)+pos_x>termSize[0]) { //lower right corner x check
        if(cursorMode==-1) cursorMode = 0;
        for(int x=menuTable.table[0].size()-1; x>=0; x--) {
            if(menuTable.getCellX(x,static_cast<int>(menuTable.table.size())-1)+pos_x<termSize[0]) {
                display_dim[1][0] = x;
                break;
            }
        }
    }
    if(menuTable.getCellX(0, 0)+pos_x<=0) { //upper left corner x check
        if(cursorMode==-1) cursorMode = 0;
        for(int x=0; x<menuTable.table[0].size(); x++) {
            if(menuTable.getCellX(x, 0)+pos_x>0) {
                display_dim[0][0] = x;
                break;
            }
        }
    }
    if(menuTable.getCellY(menuTable.table[0].size()-1,static_cast<int>(menuTable.table.size())-1)+pos_y>termSize[1]) { //lower right corner y check
        if(cursorMode==-1) cursorMode = 0;
        for(int y=static_cast<int>(menuTable.table.size())-1; y>=0; y--) {
            if(menuTable.getCellY(menuTable.table[0].size()-1, y)+pos_y<termSize[1]) {
                display_dim[1][1] = y;
                break;
            }
        }
    }
    if(menuTable.getCellY(0, 0)+pos_y<=0) { //upper left corner y check
        if(cursorMode==-1) cursorMode = 0;
        for(int y=0; y<menuTable.table.size(); y++) {
            if(menuTable.getCellY(0, 0)+pos_y>0) {
                display_dim[0][1] = y;
                break;
            }
        }
    }
    // createTable displayTable(display_dim[1][0]-display_dim[0][0], display_dim[1][1]-display_dim[0][1]);
    #ifdef DEBUG__PRINT
    debug_checkPointStr  = "6";
    #endif
    /// @brief bounding boxed table which will be displayed to terminal if cursorMode != -1
    std::vector<std::vector<std::string>> dispTable_vec(
        display_dim[1][1]-display_dim[0][1],
        std::vector<std::string>(
            display_dim[1][0]-display_dim[0][0],
            ""
        )
    );
    #ifdef DEBUG__PRINT
    debug_checkPointStr  = "7";
    #endif
    /// @brief `createTable` instance that is a cropped/ROI version of menuTable
    createTable dispTable(
        display_dim[1][0]-display_dim[0][0]+1,
        display_dim[1][1]-display_dim[0][1]+1
    );
    #ifdef DEBUG__PRINT
    debug_checkPointStr  = "8";
    #endif
    /// @brief width and height of displayed-window/dynamic-edges
    int dispDim[2] = {
        display_dim[1][0]-display_dim[0][0]+1,
        display_dim[1][1]-display_dim[0][1]+1
    };

#ifndef _WIN32
    MEVENT mouse_event;

    initscr(); //std::cout<<" check 5.1"<<std::endl; std::this_thread::sleep_for(std::chrono::seconds(1));
    cbreak(); //std::cout<<" check 5.2"<<std::endl; std::this_thread::sleep_for(std::chrono::seconds(1));
    noecho(); //std::cout<<" check 5.3"<<std::endl; std::this_thread::sleep_for(std::chrono::seconds(1));
    nodelay(stdscr, TRUE); //std::cout<<" check 5.4"<<std::endl; std::this_thread::sleep_for(std::chrono::seconds(1));
    keypad(stdscr, TRUE); //std::cout<<" check 5.5"<<std::endl; std::this_thread::sleep_for(std::chrono::seconds(1));
    scrollok(stdscr, TRUE); //std::cout<<" check 5.6"<<std::endl; std::this_thread::sleep_for(std::chrono::seconds(1));
    _ncurses_windowOpen = true;
#endif
#ifdef DEBUG__PRINT
    debug_checkPointStr  = "9";
#endif
    int c = 0;
    if(!driverFuncInit) {
        if(cursorMode!=-1) {
            currCell[0] = dispDim[0]/2;
            currCell[1] = dispDim[1]/2;
        }
        else {
            bool xPlus=true;
            bool _cellFound = false;
            for(size_t _y=0; _y<menuTable.table.size(); _y++) {
                for(size_t _x=0; _x<menuTable.table.at(_y).size(); _x++) {
                    if(menuTable.table.at(_y).at(_x)!="") {
                        currCell = pos2d<int>(_x, _y);
                        _cellFound = true;
                        break;
                    }
                }
                if(_cellFound) break;
            }
        }
    }
    int carriagePos[2] = {0, 0};

    /**
     * - Whether the cursor position is locked in the center along that axis {{x_min, x_max}, {y_min, y_nax}}; it'll become false if the distance
     * from cellPos to an absolute table edge is smaller than a pre-determined roi/bounding_box radius.
     * - When the display_dim bounding box radius is the same as the mininum distance between currCell pos and any absolute
     * edges cursLock will be true and it'll maintain center.
     *
    */
    bool cursLock[2][2]= {{false, false}, {false, false}};
    #ifdef DEBUG__PRINT
    debug_checkPointStr  = "10";
    #endif

    /// vectors to hold the indices in a range for both axes {x/columns, y/rows}
    std::vector<int> display_range[2] = {
        std::vector<int>(dispDim[0], 0),
        std::vector<int>(dispDim[1], 0)
    };

    /**
     * vector<vector<int>>`of table element {x, y} index
     * [disp_y][disp_x][menu x or y] = index
    */
    std::vector<std::vector<std::vector<int>>> displayIDX_raw(
        dispDim[1],
        std::vector<std::vector<int>>(
            dispDim[0],
            std::vector<int>(2, /*number of values {x, y}*/ 0  /*value*/)
        )
    );
    if(init_clear) std::cout << ansi_code+"2J" <<std::endl;
    
    #ifdef DEBUG__PRINT
    debug_checkPointStr  = "11";
    #endif
    bool driverInit = false;

    int init_driverCallKeys_count = init_driverCallKeys.size();
    int init_driverCallKeys_full = init_driverCallKeys_count;
    if(!driverFuncInit) driverFuncInit=true;
    while(/*(c=getch()) != 27 &&*/ !exitDriver) {
        std::this_thread::sleep_for(std::chrono::milliseconds(msDelay));
        #ifdef DEBUG__PRINT
        debug_checkPointStr  = "11.1";
        #endif
        if(init_driverCallKeys_count>0) {
            c = init_driverCallKeys.at(init_driverCallKeys_full-init_driverCallKeys_count);
            init_driverCallKeys_count--;
        }
        else {
            
#if _WIN32
            if(loopInit) {
                if(_kbhit()) {
                    c = getch();
                }
                else {
                    c = -1;
                }
            }
#else
            if(khibit()) {
                c = getch();
            }
            else {
                c = -1;
            }
#endif
        }
#if _WIN32
        if(loopInit) c = keyCheck(c);
#endif
#ifdef DEBUG__PRINT
debug_checkPointStr  = "11.2";
#endif
        if(loopFunc) {
            loopFunc(this);
            // menuTable.strExport("\n", false, "\n", " ");
        }
        //???
        if(!callFunc && c==-1 && driverInit) continue;
        if(c==-1 && loopInit && loopInit_count==0) {
            driverInit = true;
            if(callFunc) continue;
        }
        #ifdef DEBUG__PRINT
        debug_checkPointStr  = "11.3";
        #endif
        if(loopInit_count>0) { loopInit_count--; }
        if(c==-1) c=0;

        lastKeyPress = c;
        /// Hovered cell movement
        if(c==CUSTOM_KEY_LEFT) {
            currCell[0]--;
            if(currCell[0]<0) currCell[0] = menuTable.table[currCell[1]].size()-1;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="" || option_key[currCell[1]][currCell[0]]==CELLOPTSPEC_TEXT) { //skip empty cells
                for(int i=0; i<menuTable.table[currCell[1]].size(); i++) {
                    currCell[0]--;
                    if(currCell[0]<0) currCell[0] = menuTable.table[currCell[1]].size()-1;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
        else if(c==CUSTOM_KEY_RIGHT) {
            currCell[0]++;
            if(currCell[0]>(menuTable.table[currCell[1]].size()-1)) currCell[0]=0;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="" || option_key[currCell[1]][currCell[0]]==CELLOPTSPEC_TEXT) {
                for(int i=0; i<menuTable.table[currCell[1]].size(); i++) {
                    currCell[0]++;
                    if(currCell[0]>(menuTable.table[currCell[1]].size()-1)) currCell[0]=0;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
        else if(c==CUSTOM_KEY_UP) {
            currCell[1]--;
            if(currCell[1]<0) currCell[1] = static_cast<int>(menuTable.table.size())-1;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="" || option_key[currCell[1]][currCell[0]]==CELLOPTSPEC_TEXT) {
                for(int i=0; i<menuTable.table.size(); i++) {
                    currCell[1]--;
                    if(currCell[1]<0) currCell[1] = static_cast<int>(menuTable.table.size())-1;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
        else if(c==CUSTOM_KEY_DOWN) {
            currCell[1]++;
            if(currCell[1]>(static_cast<int>(menuTable.table.size())-1)) currCell[1]=0;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="" || option_key[currCell[1]][currCell[0]]==CELLOPTSPEC_TEXT) {
                for(int i=0; i<menuTable.table.size(); i++) {
                    currCell[1]++;
                    if(currCell[1]>(static_cast<int>(menuTable.table.size())-1)) currCell[1]=0;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
        // else if(c==9) { // tab
        // }
        #ifdef DEBUG__PRINT
        debug_checkPointStr  = "11.4";
        #endif
        if(loopInit && (c==CUSTOM_KEY_ENTER || c=='\n')) {
            pressedCell[0] = currCell[0];
            pressedCell[1] = currCell[1];
            if(callFunc) {
                customCall_argLess(currCell[0], currCell[1]);
                if(iterClear) std::cout << ansi_code+"2J" << ansi_code+"H"; std::cout.flush();
                loopInit = false;
                loopInit_count=1;
            }
            else {
                pressed_option[0] = currCell[0];
                pressed_option[1] = currCell[1];
                loopInit_count=1;
                loopInit = false;
                exitDriver = true;
                return pressed_option;
            }
        }

        bool subBreak = false;
        for(int y=0; y<option_key.size(); y++) {
            for(int x=0; x<option_key.at(y).size(); x++) {
                if(c==option_key.at(y).at(x)) {
                    pressedCell[0] = currCell[0];
                    pressedCell[1] = currCell[1];
                    if(callFunc) {
                        customCall_argLess(x, y);
                        if(iterClear) std::cout << ansi_code+"2J" << ansi_code+"H"; std::cout.flush();
                        loopInit = false;
                        loopInit_count=1;
                    }
                    else {
                        pressed_option[0] = x;
                        pressed_option[1] = y;
                        loopInit_count=1;
                        loopInit = false;
                        exitDriver = true;
                        return pressed_option;
                    }
                    subBreak = true;
                    break;
                }
            }
            if(subBreak) break;
        }
        #ifdef DEBUG__PRINT
        debug_checkPointStr  = "11.5.0";
        #endif
#ifndef _WIN32
        if(c==KEY_MOUSE) {
            if(getmouse(&mouse_event) == OK) {
                if(mouse_event.bstate & BUTTON1_CLICKED) { //left mouse button pressed
                    ANSI_mvprint(100, 1, "MOUSE PRESSED");
                    std::cin.get();
                    std::cin.ignore();
                    std::cin.clear();
                    bool buttonFound = false;
                    for(int y=0; y<menuTable.table.size(); y++) {
                        for(int x=0; x<menuTable.table[y].size(); x++) {
                            int relCellPos[2] = {menuTable.getCellX(x,y), menuTable.getCellY(x,y)};
                            int cellLen = menuTable.maxColumnLen[x];
                            if(
                                ( mouse_event.x>=relCellPos[0] && mouse_event.y<=relCellPos[0]+cellLen ) &&
                                ( mouse_event.y==relCellPos[1] )
                            ) {
                                pressedCell[0] = currCell[0];
                                pressedCell[1] = currCell[1];
                                if(callFunc) {
                                    customCall_argLess(x, y);
                                    loopInit = false;
                                    loopInit_count=1;
                                }
                                else {
                                    pressed_option[0] = x;
                                    pressed_option[1] = y;
                                    loopInit_count=1;
                                    loopInit = false;
                                    exitDriver = true;
                                    return pressed_option;
                                }
                                buttonFound = true;
                                break;
                                if(iterClear) std::cout << ansi_code+"2J";
                            }
                        }
                        
                        if(buttonFound) break;
                    }
                }
            }
        }
#endif
        #ifdef DEBUG__PRINT
        debug_checkPointStr  = "11.5.0.1";
        #endif
        /// cursorMode based movement drawing.
        if(cursorMode==-1) {
            ansiPrint(
                menuTable.exportStr,
                static_cast<int>(pos_x),
                static_cast<int>(pos_y)
            );
            //locate terminal pos of highlighted cell in respect to cell container 2d location
            carriagePos[0] = menuTable.getCellX(currCell[0], currCell[1]);
            carriagePos[1] = menuTable.getCellY(currCell[0], currCell[1]);
            //display/highlight the currentCell that is hovered over.
            ANSI_mvprint(
                carriagePos[0]+pos_x,
                carriagePos[1]+pos_y,
                ansi_code+highlight_textColour+";"+highlight_bgColour+"m"+
                menuTable.table.at(currCell[1]).at(currCell[0])+std::string(menuTable.columnPadding.at(currCell[0]), ' ')
                +ansi_code+bg_textColour+";"+bg_bgColour+"m"
            );
        }
        if(cursorMode==0) {
            int diff=0;
            if(currCell[0]<display_dim[0][0]) {
                diff = currCell[0]-display_dim[0][0];
                display_dim[0][0] += diff;
                display_dim[1][0] += diff;
            }
            else if(currCell[0]>=display_dim[1][0]) {
                diff = currCell[0]-display_dim[1][0];
                display_dim[0][0] += diff;
                display_dim[1][0] += diff;
            }
            if(currCell[1]<display_dim[0][1]) {
                diff = currCell[1]-display_dim[0][1];
                display_dim[0][1] += diff;
                display_dim[1][1] += diff;
            }
            else if(currCell[1]>=display_dim[1][1]) {
                diff = currCell[1]-display_dim[1][1];
                display_dim[0][1] += diff;
                display_dim[1][1] += diff;
            }
            // ANSI_mvprint(
            //     0, 0, 
            //     "{{"+
            //     std::to_string(display_dim[0][0])+
            //     ", "+
            //     std::to_string(display_dim[0][1])+
            //     "},{"+
            //     std::to_string(display_dim[1][0])+
            //     ","+
            //     std::to_string(display_dim[1][1])+
            //     "}}"+
            //     " [" +
            //     std::to_string(currCell[0])+
            //     ", "+
            //     std::to_string(currCell[1])+
            //     "]"
            // );

            for(int y=display_dim[0][1]; y<=display_dim[1][1]; y++) {
                for(int x=display_dim[0][0]; x<=display_dim[1][0]; x++) {
                    dispTable.insertText(
                        menuTable.table.at(y).at(x),
                        x-display_dim[0][0], y-display_dim[0][1]
                    );
                }
            }
            
            menuTable.copyTo_settings(dispTable);
            dispTable.strExport("\n", false, "\n", " ");
            ansiPrint(
                dispTable.exportStr,
                static_cast<int>(pos_x),
                static_cast<int>(pos_y)
            );
            carriagePos[0] = menuTable.getCellX(currCell[0]-display_dim[0][0], currCell[1]-display_dim[0][1]);
            carriagePos[1] = menuTable.getCellY(currCell[0]-display_dim[0][0], currCell[1]-display_dim[0][1]);

            //display/highlight the currentCell that is hovered over.
            std::string colCode0= ansi_code+highlight_textColour+";"+highlight_bgColour+"m";
            std::string colCode1= ansi_code+bg_textColour+";"+bg_bgColour+"m";
            std::string mainStr = menuTable.table.at(currCell[1]).at(currCell[0]);
            mainStr += std::string(button_totWidth*menuTable.maxColumnLen.at(currCell[0]), ' ');

            ANSI_mvprint(
                carriagePos[0]+pos_x,
                carriagePos[1]+pos_y,
                colCode0+
                mainStr+
                colCode1
            );
        }
        if(cursorMode==1) {

            for(int i=0; i<2; i++) {
                for(int ii=0; ii<2; ii++) {
                    cursLock[i][ii] = false;
                }
                dispDim[i] = display_dim[1][i]-display_dim[0][i];
            }

            // Check if the ROI/bounding box is colliding with absolute edges/table
            if(currCell[0]-dispDim[0]/2>=0) cursLock[0][0] = true;
            if(currCell[0]+dispDim[0]/2<=menuTable.table[0].size()-1) cursLock[1][0] = true;
            if(currCell[1]-dispDim[1]/2>=0) cursLock[0][1] = true;
            if(currCell[1]+dispDim[1]/2<=static_cast<int>(menuTable.table.size())-1) cursLock[1][1] = true;


            /// Set display/dymanic-edges width parameters
            if(cursLock[0][0] && cursLock[1][0]) {
                display_dim[0][0] = currCell[0]-dispDim[0]/2;
                display_dim[1][0] = currCell[0]+dispDim[0]/2;
            }
            else {
                if(!cursLock[0][0]) {
                    display_dim[0][0] = 0;
                    display_dim[1][0] = dispDim[0];
                }
                else if(!cursLock[1][0]) {
                    display_dim[1][0] = menuTable.table[0].size()-1;
                    display_dim[0][0] = menuTable.table[0].size()-1 - dispDim[0];
                }
            }


            /// Set display/dymanic-edges height parameters
            if(cursLock[0][1] && cursLock[1][1]) {
                display_dim[0][1] = currCell[1]-dispDim[1]/2;
                display_dim[1][1] = currCell[1]+dispDim[1]/2;
            }
            else {
                if(!cursLock[0][1]) {
                    display_dim[0][1] = 0;
                    display_dim[1][1] = dispDim[1];
                }
                else if(!cursLock[1][1]) {
                    display_dim[1][1] = static_cast<int>(menuTable.table.size())-1;
                    display_dim[0][1] = static_cast<int>(menuTable.table.size())-1 - dispDim[1];
                }
            }

            /// fill in Display roi's createTable object cells with defined corner parmeters
            for(int y=display_dim[0][1]; y<=display_dim[1][1]; y++) {
                for(int x=display_dim[0][0]; x<=display_dim[1][0]; x++) {
                    dispTable.insertText(
                        menuTable.table.at(y).at(x),
                        x-display_dim[0][0], y-display_dim[0][1]
                    );
                }
            }


            /// copy settings from absolute table to display table and export display table
            menuTable.copyTo_settings(dispTable);
            dispTable.strExport("\n", false, "\n", " ");

            /// print display table
            ansiPrint(
                dispTable.exportStr,
                static_cast<int>(pos_x),
                static_cast<int>(pos_y)
            );

            /// get the absolute terminal cursor position/coordinate for highlighted cell
            carriagePos[0] = menuTable.getCellX(currCell[0]-display_dim[0][0], currCell[1]-display_dim[0][1]);
            carriagePos[1] = menuTable.getCellY(currCell[0]-display_dim[0][0], currCell[1]-display_dim[0][1]);

            /// display/highlight the currentCell that is hovered over.
            ANSI_mvprint(
                carriagePos[0]+pos_x,
                carriagePos[1]+pos_y,
                ansi_code+highlight_textColour+";"+highlight_bgColour+"m"+
                menuTable.table.at(currCell[1]).at(currCell[0])+std::string(menuTable.columnPadding.at(currCell[0]), ' ')
                +ansi_code+bg_textColour+";"+bg_bgColour+"m"
            );

        }
        if(cursorMode==2) {
            // /**
            //  * Same as cursorMode 1 but the cursor doesn't unlock from the center. Instead
            //  * the display table fills in the missing cells with with what is supposed to be there
            //  * according to the highlighted cell's coordinate relative to the absolute table
            //  * 
            //  */
        
            /// fill in matrix of indices of menuTable coordinatee valueswhatthefuckamairamblingabout
            for(size_t y=0; y<displayIDX_raw.size(); y++) {
                for(size_t x=0; x<displayIDX_raw[0].size(); x++) {
                    displayIDX_raw[y][x][0] = currCell[0]-dispDim[0]/2+x;
                    displayIDX_raw[y][x][1] = currCell[1]-dispDim[1]/2+y;

                    // ANSI_mvprint(
                    //     10, 4,
                    //     "{"+std::to_string(displayIDX_raw[y][x][0])+":"+std::to_string(displayIDX_raw[y][x][1])+"}"
                    // );

                    if(displayIDX_raw[y][x][0]<0) displayIDX_raw[y][x][0] = menuTable.table[0].size()+displayIDX_raw[y][x][0];
                    else if(displayIDX_raw[y][x][0]>menuTable.table[0].size()-1) displayIDX_raw[y][x][0] -= menuTable.table[0].size();

                    if(displayIDX_raw[y][x][1]<0) displayIDX_raw[y][x][1] = menuTable.table.size()+displayIDX_raw[y][x][1];
                    else if(displayIDX_raw[y][x][1]>static_cast<int>(menuTable.table.size())-1) displayIDX_raw[y][x][1] -= menuTable.table.size();
                    
                    // ANSI_mvprint(
                    //     20, 4,
                    //     "{"+std::to_string(displayIDX_raw[y][x][0])+":"+std::to_string(displayIDX_raw[y][x][1])+"}"
                    // );

                    dispTable.insertText(
                        menuTable.table.at(displayIDX_raw[y][x][1]).at(displayIDX_raw[y][x][0]),
                        x, y
                    );
                }
            }
            

            // ANSI_mvprint(1, 5, "check 5");

            /// copy settings from absolute table to display table and export display table
            menuTable.copyTo_settings(dispTable);
            dispTable.strExport("\n", false, "\n", " ");

            
            // ANSI_mvprint(1, 6, "check 6");

            /// print display table
            ansiPrint(
                dispTable.exportStr,
                static_cast<int>(pos_x),
                static_cast<int>(pos_y)
            );


            // ANSI_mvprint(1, 7, "check 7");

            /// get the absolute terminal cursor position/coordinate for highlighted cell
            if(!loopInit) {
                carriagePos[0] = menuTable.getCellX(currCell[0], currCell[1]);
                carriagePos[1] = menuTable.getCellY(currCell[0], currCell[1]);
            }
            // ANSI_mvprint(1, 8, "check 8");

            /// display/highlight the currentCell that is hovered over.
            ANSI_mvprint(
                carriagePos[0]+pos_x,
                carriagePos[1]+pos_y,
                ansi_code+highlight_textColour+";"+highlight_bgColour+"m"+
                menuTable.table.at(currCell[1]).at(currCell[0])+std::string(menuTable.columnPadding.at(currCell[0]), ' ')
                +ansi_code+bg_textColour+";"+bg_bgColour+"m"
            );

            // ANSI_mvprint(1, 9, "check 9");

        }
        
        #ifdef DEBUG__PRINT
        debug_checkPointStr  = "11.6";
        #endif
#ifndef _WIN32
        refresh();
#endif
        if(!loopInit) loopInit=true;
    }
    loopInit = false;
    #ifdef DEBUG__PRINT
    debug_checkPointStr  = "12";
    #endif
    ANSI_mvprint(termSize[0]-10, 1, "exited");
    // std::cin.get();
    // std::cin.ignore();
    // std::cin.clear();
    #ifdef DEBUG__PRINT
    debug_checkPointStr  = "13";
    #endif
#if _WIN32
    std::cout << ansi_code+"2J" <<std::endl;
#else
    endwin();
#endif
#ifdef DEBUG__PRINT    
debug_checkPointStr  = "14";
#endif
    return pressed_option;
}

void TUI::termMenu::updateTable() {
    menuTable.strExport("\n", false, "\n", " ");
}

std::string TUI::termMenu::termInput(
    int eventMethod,
    int pos_x,
    int pos_y,
    int bar_width,
    int bar_height,
    std::string textAlign,
    std::string text_colour,
    std::string bg_colour,
    std::string startText,
    bool *tabCalled
) {
    (*tabCalled) = false;
    if(bar_width==0) bar_height = 1;

    getTermSize(termSize[0], termSize[1]);
    std::string inputStr = "";

    if(pos_x>termSize[0]) pos_x = 0;
    if(pos_y>termSize[1]) pos_y = 0;

    if(pos_x+bar_width>termSize[0]) pos_x = termSize[0]-bar_width;
    if(pos_y+bar_height>termSize[1]) pos_y = termSize[1]-bar_height;

    std::string prev_textColour = "";
    std::string prev_bgColour = "";

    
    if(pos_x != -1 && pos_y != -1) {
        /*set new colours*/
        
        if(bar_height>0 && bar_width>0) {
            for(int y=pos_y; y<pos_y+bar_height; y++) {
                ANSI_mvprint(
                    pos_x, y, 
                    ansi_code+text_colour+";"+bg_colour+"m"+std::string(bar_width, ' ')
                );
            }
        }
        ANSI_mvprint(
            pos_x-startText.length(), pos_y, 
            ansi_code+text_colour+";"+bg_colour+"m"+startText
        );
    }


    int x_offset = 0, y_offset = 0;

    // if(eventMethod==0) {
    //     std::cin.ignore();
    //     std::cin.clear();
    //     echo();
    //     nocbreak();
    //     refresh();
    //     std::getline(std::cin, inputStr);
    //     std::cin.ignore();
    //     std::cin.clear();
    //     cbreak();
    //     noecho();
    //     refresh();
    // }
    if(eventMethod==1) {
        int key_c=0;
        std::string outStr = "";
        // nodelay(stdscr, FALSE);
        while((key_c=getch()) != '\n') {
            if(key_c==-1) continue;

            if(pos_x!=-1 && pos_y!=-1) {
                if(bar_height>0 && bar_width>0) {
                    for(int y=pos_y; y<pos_y+bar_height; y++) {
                        ANSI_mvprint(pos_x, y, ansi_code+text_colour+";"+bg_colour+"m"+std::string(bar_width, ' ') );
                    }
                }
            }

            // if(key_c=='\n') break;
            if(key_c==CUSTOM_KEY_BACKSPACE && inputStr.length()>0) inputStr.erase(inputStr.length()-1);
            else if(key_c==9) {
                //inputStr+=std::string(4, ' ');
                (*tabCalled) = true;
                break;
            }
            else if(key_c==27) break;

            else inputStr += char(key_c);
            

            if(bar_height>0 && bar_width>0) {
                if(inputStr.length()>bar_height*bar_width) outStr = inputStr.substr(inputStr.length()-bar_height*bar_width);
                else outStr = inputStr;

                for(int row=0; row<=floor(static_cast<float>(outStr.length())/bar_width); row++) {
                    ANSI_mvprint(
                        pos_x, pos_y+row,
                        ""+outStr.substr(row*bar_width, bar_width)
                    );
                }
                if(inputStr.length()>=bar_height*bar_width) ANSI_mvprint(pos_x+bar_width, pos_y+bar_height-1, "");
            }

            // y_offset = inputStr.length()/bar_width;
            // ANSI_mvprint(
            //     pos_x-std::string("input:").length(), pos_y+y_offset,
            //     ansi_code+text_colour+";"+bg_colour+"minput:"+inputStr
            // );
        }
        // nodelay(stdscr, TRUE);
    }
    ANSI_mvprint(
        pos_x, pos_y,
        ansi_code+prev_textColour+";"+prev_bgColour+"m"
    );

    return inputStr;
}


int TUI::termMenu::redefine_function(int x_column, int y_row, TDEF_void__ newFunc) {
    if(x_column>=menuTable.table[0].size() || y_row>=menuTable.table.size()) return 1;
    optFunc_id0[x_column][y_row] = newFunc;

    return 0;
}

int TUI::termMenu::redefine_opt_id0_id1(int x_column, int y_row, TDEF_void_pBool optFunc, bool* param0) {
    if(x_column>=menuTable.table[0].size() || y_row>=menuTable.table.size()) return 1;
    
    optFunc_id0.at(y_row).at(x_column) = termMenu_nullFunc_void__;
    optFunc_id1_.at(y_row).at(x_column) = optFunc;
    optFunc_id1_p0.at(y_row).at(x_column) = param0;
    optFunc_identifier.at(y_row).at(x_column) = 1;

    return 0;
}

int TUI::termMenu::getCellPos_x(int column, int row, std::string rowSep) {
    return menuTable.getCellX(column,row,rowSep);
}

int TUI::termMenu::getCellPos_y(int column, int row, std::string rowSep) {
    return menuTable.getCellY(column,row,rowSep);
}

std::string TUI::termMenu::getCellName(int column, int row) {
    return menuTable.table.at(row).at(column);
}


void TUI::termMenu::init_driverCallKeys_add(int key, int addLoc) {
    if(addLoc<0 || addLoc>=init_driverCallKeys.size()) init_driverCallKeys.push_back(key);
    else {
        std::vector<int>::iterator itr = init_driverCallKeys.begin() + addLoc;
        init_driverCallKeys.insert(itr, key);
    }
}
void TUI::termMenu::init_driverCallKeys_add(std::vector<int> keys, int addLoc) {
    if(addLoc<0 || addLoc>init_driverCallKeys.size()-1) addLoc = init_driverCallKeys.size();
    std::vector<int>::iterator itr = init_driverCallKeys.begin() + addLoc;
    init_driverCallKeys.insert(itr, keys.begin(), keys.end());
}

void TUI::termMenu::init_driverCallKeys_clear() {
    init_driverCallKeys.clear();
}


void TUI::termMenu::moveCurrCell(size_t col, size_t row) {

}
void TUI::termMenu::moveCurrCell(int xDir, int yDir) {

    if(xDir<0) {
        for(int x=xDir; x<0; x++) {
            currCell[0]--;
            if(currCell[0]<0) currCell[0] = menuTable.table[currCell[1]].size()-1;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="" || option_key[currCell[1]][currCell[0]]==CELLOPTSPEC_TEXT) { //skip empty cells
                for(int i=0; i<menuTable.table[currCell[1]].size(); i++) {
                    currCell[0]--;
                    if(currCell[0]<0) currCell[0] = menuTable.table[currCell[1]].size()-1;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
    }
    else if(xDir>0) {
        for(int x=0; x<xDir; x++) {
            currCell[0]++;
            if(currCell[0]>(menuTable.table[currCell[1]].size()-1)) currCell[0]=0;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="" || option_key[currCell[1]][currCell[0]]==CELLOPTSPEC_TEXT) {
                for(int i=0; i<menuTable.table[currCell[1]].size(); i++) {
                    currCell[0]++;
                    if(currCell[0]>(menuTable.table[currCell[1]].size()-1)) currCell[0]=0;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
    }
    if(yDir>0) {
        for(int y=0; y<yDir; y++) {
            currCell[1]++;
            if(currCell[1]>(static_cast<int>(menuTable.table.size())-1)) currCell[1]=0;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="" || option_key[currCell[1]][currCell[0]]==CELLOPTSPEC_TEXT) {
                for(int i=0; i<menuTable.table.size(); i++) {
                    currCell[1]++;
                    if(currCell[1]>(static_cast<int>(menuTable.table.size())-1)) currCell[1]=0;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
    }
    else if(yDir<0) {
        for(int y=yDir; y<0; y++) {
            currCell[1]--;
            if(currCell[1]<0) currCell[1] = static_cast<int>(menuTable.table.size())-1;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="" || option_key[currCell[1]][currCell[0]]==CELLOPTSPEC_TEXT) {
                for(int i=0; i<menuTable.table.size(); i++) {
                    currCell[1]--;
                    if(currCell[1]<0) currCell[1] = static_cast<int>(menuTable.table.size())-1;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
    }

}


int TUI::termMenu::getMenu_width() {
    if(!menuTable.read_exportCalled()) {
        int tempWidth = static_cast<int>(array_sum(menuTable.maxColumnLen));
        return tempWidth+menuTable.maxColumnLen.size();
    }
    int result=0;
    size_t firstNL = menuTable.exportStr.find('\n');
    size_t secondNL = menuTable.exportStr.find(firstNL+1, '\n');
    if(firstNL != secondNL-(firstNL+1)) {
        /**
         * strExport() allows a string insertion at the start which doesn't require same table width
         */
        result = secondNL-firstNL-1;
    }
    else { result = firstNL; }

    return result;
}

int TUI::termMenu::getMenu_height() {
    if(!menuTable.read_exportCalled()) {
        return menuTable.table.size();
    }
    return findOcc_char(menuTable.exportStr, '\n');
}


bool TUI::termMenu_nullParam_bool = false;
int  TUI::termMenu_nullParam_int = 0;
char TUI::termMenu_nullParam_char = '0';
std::string TUI::termMenu_nullParam_string = "0";
