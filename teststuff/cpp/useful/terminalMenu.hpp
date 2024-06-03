#pragma once
#ifndef HPP_TERMINAL_MENU
#define HPP_TERMINAL_MENU


#define COLOUR_TEXT_BLACK   "30" 
#define COLOUR_TEXT_RED     "31"
#define COLOUR_TEXT_GREEN   "32"
#define COLOUR_TEXT_YELLOW  "33"
#define COLOUR_TEXT_BLUE    "34"
#define COLOUR_TEXT_MAGENTA "35"
#define COLOUR_TEXT_CYAN    "36"
#define COLOUR_TEXT_WHITE   "37"

#define COLOUR_BG_BLACK     "40" 
#define COLOUR_BG_RED       "41"
#define COLOUR_BG_GREEN     "42"
#define COLOUR_BG_YELLOW    "43"
#define COLOUR_BG_BLUE      "44"
#define COLOUR_BG_MAGENTA   "45"
#define COLOUR_BG_CYAN      "46"
#define COLOUR_BG_WHITE     "47"



#include <iostream>
#include <ncurses.h>
#include <vector>
#include <algorithm>
#include <cmath>

#include <HC_useful/createTable.hpp>
#include <HC_useful/useful.hpp>


/// @brief placeholder function for empty function parameter
void termMenu_nullFunc_void__() {}
void termMenu_nullFunc_void_pBool(bool*) {}
void termMenu_nullFunc_void_pInt(int*) {}
void termMenu_nullFunc_void_pChar(char*) {}
void termMenu_nullFunc_void_pStr(std::string*) {}

bool termMenu_nullParam_bool = false;
int termMenu_nullParam_int = 0;
char termMenu_nullParam_char = '0';
std::string termMenu_nullParam_string = "0";



class termMenu {


    int currCell[2] = {0, 0}; //"cursor" pos
    bool driverFuncInit = false; // init boolean for whether the driver has been called: class scope
    bool classInit = false;
    bool callFunc = false;

    std::vector<int> init_driverCallKeys; // `std::vector<int>` container of keyboard key(/integers) to call during `termMenu::driver()` function call.


    createTable menuTable;
    bool init_table = false;
    int option_width = 0;

    int button_totWidth = -1;

    typedef void    (*TDEF_void__)();               /*id:`0`: type `void ()`*/
    typedef void    (*TDEF_void_pBool)(bool*);      /*id:`1`: type `void (bool*)`*/
    typedef void    (*TDEF_void_pInt)(int*);        /*id:`2`: type `void (int*)*/
    typedef void    (*TDEF_void_pChar)(char*);      /*id:`3`: type `void (char*)`*/
    typedef void    (*TDEF_void_pStr)(std::string*);/*id:`4`: type `void (std::string*)`*/

    // typedef void    (*TDEF_void_Bool)(bool); /*id:`2`: type `void (bool) `*/
    

    /**
     * id: `0`
     * type: `void ()`
     * 
     * 2d vector container for functions to call in accordance to menu option chosen
     * [rows][columns]
     */
    std::vector<std::vector<TDEF_void__>> optFunc_id0;

    /**
     * id: `1`
     * type: `void (bool*)`
     * 
     * 2d vector container for functions to call in accordance to menu option chosen
     * [rows][columns]
    */
    std::vector<std::vector<TDEF_void_pBool>> optFunc_id1_;
    /**
     * base container: `termMenu::optFunc_id1_`
     * parameter index: `0`
     * 
     * Parameter associated with 2d container for function
    */
    std::vector<std::vector<bool*>> optFunc_id1_p0;

    /**
     * id: `2`
     * type: `void (int*)`
     * 
     * 2d vector container for functions to call in accordance to menu option chosen
     * [rows][columns]
    */
    std::vector<std::vector<TDEF_void_pInt>> optFunc_id2_;/**
     * base container: `termMenu::optFunc_id2_`
     * parameter index: `0`
     * 
     * Parameter associated with 2d container for function
    */
    std::vector<std::vector<int*>> optFunc_id2_p0;

    /**
     * id: `3`
     * type: `void (char*)`
     * 
     * 2d vector container for functions to call in accordance to menu option chosen
     * [rows][columns]
    */
    std::vector<std::vector<TDEF_void_pChar>> optFunc_id3_;/**
     * base container: `termMenu::optFunc_id3_`
     * parameter index: `0`
     * 
     * Parameter associated with 2d container for function
    */
    std::vector<std::vector<char*>> optFunc_id3_p0;

    /**
     * id: `4`
     * type: `void (std::string*)`
     * 
     * 2d vector container for functions to call in accordance to menu option chosen
     * [rows][columns]
    */
    std::vector<std::vector<TDEF_void_pStr>> optFunc_id4_;/**
     * base container: `termMenu::optFunc_id4_`
     * parameter index: `0`
     * 
     * Parameter associated with 2d container for function
    */
    std::vector<std::vector<std::string*>> optFunc_id4_p0;


    /**
     * Identifier to specify what `TDEF_` type that 2d array cell is.
     * `-1` means that cell isn't occupied/used as an option/button
     * [y][x]
     * 
     * type id's:
     * - `0` - `void ()`
     * - `1` - `void (bool*)`
     * - `2` - `void (int*)`
     * - `3` - `void (char*)`
     * - `4` - `void (std::string)`
    */
    std::vector<std::vector<int>> optFunc_identifier;


    /**
     * Function used to extend all 2d container vectors with given amount, including
     * - `menuTable`
     * - `option_key`
     * - `optFunc_id{}`
     * @param xAdd number of columns/x-val to extend by
     * @param yAdd number of rows/y-val to extend by
     * 
    */
    void extend_containers(int xAdd, int yAdd) {
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
    int container_insertKey(int keyPress, int x_column, int y_row) {
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
    int customCall_argLess(int pos_x, int pos_y) {
        switch (optFunc_identifier.at(pos_y).at(pos_x)) {
        case 0: optFunc_id0.at(pos_y).at(pos_x)(); break;
        case 1: optFunc_id1_.at(pos_y).at(pos_x)(optFunc_id1_p0.at(pos_y).at(pos_x)); break;
        case 2: optFunc_id2_.at(pos_y).at(pos_x)(optFunc_id2_p0.at(pos_y).at(pos_x)); break;
        case 3: optFunc_id3_.at(pos_y).at(pos_x)(optFunc_id3_p0.at(pos_y).at(pos_x)); break;
        case 4: optFunc_id4_.at(pos_y).at(pos_x)(optFunc_id4_p0.at(pos_y).at(pos_x)); break;
        default:
            return -1;
        }

        return 0;
    }


    /**
     * @brief 2d vector container for the key button calls to react to inorder to call that option
     * [rows][columns]
    */
    std::vector<std::vector<int>> option_key;

    /**
     * Corner locations ({upper left, lower right}) of table for the bounding box that tells what to display
     * to the terminal.
     */
    int display_dim[2][2] = {
        {0, 0},
        {1, 1}
    };

    /**
     * What mode the cursor will be going to the different options/button by.
     * `-1` - "unbound": don't bound the absolute table and instead show the complete table to the terminal.
     * 
     * `0`  - "free": as in the cursor isn't bound and will move single space/cell at a time. The options
     * outside the "dynamic edges"(edges of the visible table's bounding box) are accessed by moving the cursor
     * beyond that "hiding edge".
     *
     * `1`  - "center": the cursor is centered on the displayed table unless the displayed table bounding box
     * enters beyond the "absolute edge" (edges for all the table options).
     * 
     * `2` - "centerFree": the cursor is centered at all time and when the dynamic edges(display table) reach
     * absolute edges(absolute table) then it'll fill in the missing spaces with the opposite side values
    */
    int cursorMode = -1;

    public:
    const std::string ansi_code = "\x1B[";

    
    int lastKeyPress = 0;
    /// @brief boolean for whether to exit `termMenu::driver()` main while loop
    bool exitDriver = true;

    int termLoc[2] = {0, 0};
    int termSize[2] = {0, 0};

    termMenu(bool callFromDriver, int menuColumns, int menuRows);

    std::string termInput(
        int eventMethod,
        int pos_x, int pos_y,
        int bar_width, int bar_height,
        std::string textAlign,
        std::string text_colour, std::string bg_colour,
        std::string startText,
        bool *tabCalled
    );

    int setButtonWidth(int width);
    int setDisplayDim(int c0_x, int c0_y, int c1_x, int c1_y, int cursMode);

    int addOpt(std::string optName, int x_column, int y_row, int keyPress, TDEF_void__ optFunc);
    int addOpt(std::string optName, int x_column, int y_row, int keyPress, TDEF_void_pBool optFunc, bool* param0);
    int addOpt(std::string optName, int x_column, int y_row, int keyPress, TDEF_void_pInt optFunc, int* param0);
    int addOpt(std::string optName, int x_column, int y_row, int keyPress, TDEF_void_pChar optFunc, char* param0);
    int addOpt(std::string optName, int x_column, int y_row, int keyPress, TDEF_void_pStr optFunc, std::string* param0);

    const int* driver(
        int pos_x, int pos_y,
        int msDelay,
        bool init_clear,
        TDEF_void__ loopFunc,
        bool iterClear,
        std::string highlight_textColour,
        std::string highlight_bgColour,
        std::string bg_textColour,
        std::string bg_bgColour
    );

    void init_driverCallKeys_add(int key, int addLoc);
    void init_driverCallKeys_add(std::vector<int> keys, int addLoc);
    void init_driverCallKeys_clear();

    void moveCurrCell(size_t col, size_t row);
    void moveCurrCell(int xDir, int yDir);

    int redefine_function(int x_column, int y_row, TDEF_void__ newFunc);
    int redefine_opt_id0_id1(int x_column, int y_row, TDEF_void_pBool optFunc, bool* param0);
    int rename_opt(int x_column, int y_row, std::string new_optName);

    int getCellPos_x(int column, int row, std::string rowSep);
    int getCellPos_y(int column, int row, std::string rowSep);
    std::string getCellName(int column, int row);

    int getMenu_width();
    int getMenu_height();
};


/**
 * @brief class constructor. Must be called before using class instances
 * @param callFromDriver whether to call a specific function during an option
 * selection or to return table "coordinate" 
 * @param menuColumns number of columns to initialise menu with
 * @param menuRows number of rows to initialise menu with
*/
termMenu::termMenu(
    bool callFromDriver,
    int menuColumns = 1,
    int menuRows = 1
) {
    if(menuColumns<1 || menuRows<1) {
        std::cerr << "ERROR: termMenu init: menu{Columns/Rows} can't be smaller than 1"<<std::endl;
        return;
    }
    classInit = true;
    callFunc = callFromDriver;

    
    extend_containers(
        menuColumns,
        menuRows
    );
}

/**
 * Set the dimensions of what is displayed of the total table to the terminal (like a Region_Of_Interest/Bounding_box)
 * @param c0_x table x/column value of upper left corner for displayed box
 * @param c0_y table y/row value of upper left corner for displayed box
 * @param c1_x table x/column value of lower right corner for displayed box
 * @param c1_y table y/row value of lower right corner for displayed box
 * @param cursMode What mode the cursor will be moving between the different options/buttons by:
 * - `-1` - "unbound": don't create a display bounding box around the full table but instead display the
 * full table.
 * 
 * - `0`  - "free": as in the cursor isn't bound and will move single space/cell at a time. The options
 * outside the "dynamic edges"(edges of the visible table's bounding box) are accessed by moving the cursor
 * beyond that "hiding edge".
 *
 * - `1`  - "center": the cursor is centered on the displayed table unless the displayed table bounding box
 * enters beyond the "absolute edge" (edges for all the table options)
 * 
 * - `2` - "centerFree": the cursor is centered at all time and when the dynamic edges(display table) reach
 * absolute edges(absolute table) then it'll fill in the missing spaces with the opposite side values
 * 
 * @return whether the function was successful in its goal; `0` - successful; `-1` - unsuccessful/error-occurred
 * @note the corner values `c{id}_{axis}` represent a cellPos that is inside the bounding box;
 * @note if any `c{id}_{axis}` parameter is set to `-1` then the displayDim will be set to unlocked/bound and will instead
 * display the entire table (if possible). If the tables' corners are outside the terminal it's display on then it'll
 * automatically lock/bound the displayed table to something that fits the terminal with cursMode default to 0.
*/
int termMenu::setDisplayDim(
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
        // display_dim[1][0] = menuTable.getCellX(menuTable.table.at(0).size()-1, menuTable.table.size()-1);
        // display_dim[1][1] = menuTable.getCellY(menuTable.table.at(0).size()-1, menuTable.table.size()-1);
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

/**
 * @brief set width of all buttons/options
 * @param width width of the button
 * @return 0 if successful; -1 if included width was too small (smaller than text length)
*/
int termMenu::setButtonWidth(int width) {
    if(width-menuTable.getColTotMax()<0) return -1;

    button_totWidth = width;
    // menuTable.setColPadding(std::vector<int>(menuTable.table.at(0).size(), width));

    return 0;
}

/**
 * @brief add/set/edit an option/button info
 * @param optName `std::string` variable of name for the button
 * @param x_column x-coordinate/column location button/option to add/set/edit
 * @param y_row y-coordinate/row location of button/option to add/set/edit
 * @param keyPress int of the key event to react to: if `-1` then it will only be called with button press.
 * @param optFunc pointer to `void ()` function to call if that button/option is pressed/selected
 * @note - If `x_column`/`y_row` are further out than the already created dimensions (ex. there are less columns than x_column calls)
 * @note then it'll expand the 2d-container/table dimension along that axis.
 * @note - If `keyPress` isn't `-1` and that keyPress has already been "used" it'll set that keyPress to `-1` and return function -1
 * @return `0` if successfully added: `1` if an error occurred trying to addOpt
*/
int termMenu::addOpt(
    std::string optName,
    int x_column = 0,
    int y_row = 0,
    int keyPress = -1,
    TDEF_void__ optFunc = termMenu_nullFunc_void__
) {

    extend_containers(
        x_column-(menuTable.table.at(0).size()-1),
        y_row-(menuTable.table.size()-1)
    );
    if(container_insertKey(keyPress, x_column, y_row)==-1) return 1;
    menuTable.insertText(optName, x_column, y_row);


    optFunc_identifier.at(y_row).at(x_column) = 0;
    optFunc_id0.at(y_row).at(x_column) = optFunc;

    return 0;
}

/**
 * @brief add/set/edit an option/button info
 * @param optName `std::string` variable of name for the button
 * @param x_column x-coordinate/column location button/option to add/set/edit
 * @param y_row y-coordinate/row location of button/option to add/set/edit
 * @param keyPress int of the key event to react to: if `-1` then it will only be called with button press.
 * @param optFunc pointer to `void (bool*)` function to call if that button/option is pressed/selected
 * @param param0 pointer to boolean var to pass to optFunc
 * @note - If `x_column`/`y_row` are further out than the already created dimensions (ex. there are less columns than x_column calls)
 * @note then it'll expand the 2d-container/table dimension along that axis.
 * @note - If `keyPress` isn't `-1` and that keyPress has already been "used" it'll set that keyPress to `-1` and return function -1
 * @return `0` if successfully added: `1` if an error occurred trying to addOpt
*/
int termMenu::addOpt(
    std::string optName,
    int x_column = 0,
    int y_row = 0,
    int keyPress = -1,
    TDEF_void_pBool optFunc = termMenu_nullFunc_void_pBool,
    bool* param0 = &termMenu_nullParam_bool
) {

    extend_containers(
        x_column-(menuTable.table.at(0).size()-1),
        y_row-(menuTable.table.size()-1)
    );
    if(container_insertKey(keyPress, x_column, y_row)==-1) return 1;
    menuTable.insertText(optName, x_column, y_row);


    optFunc_identifier.at(y_row).at(x_column) = 1;
    optFunc_id1_.at(y_row).at(x_column) = optFunc;
    optFunc_id1_p0.at(y_row).at(x_column) = param0;

    return 0;
}
/**
 * @brief add/set/edit an option/button info
 * @param optName `std::string` variable of name for the button
 * @param x_column x-coordinate/column location button/option to add/set/edit
 * @param y_row y-coordinate/row location of button/option to add/set/edit
 * @param keyPress int of the key event to react to: if `-1` then it will only be called with button press.
 * @param optFunc pointer to `void (int*)` function to call if that button/option is pressed/selected
 * @param param0 pointer to integer var to pass to optFunc
 * @note - If `x_column`/`y_row` are further out than the already created dimensions (ex. there are less columns than x_column calls)
 * @note then it'll expand the 2d-container/table dimension along that axis.
 * @note - If `keyPress` isn't `-1` and that keyPress has already been "used" it'll set that keyPress to `-1` and return function -1
 * @return `0` if successfully added: `1` if an error occurred trying to addOpt
*/
int termMenu::addOpt(
    std::string optName,
    int x_column = 0,
    int y_row = 0,
    int keyPress = -1,
    TDEF_void_pInt optFunc = termMenu_nullFunc_void_pInt,
    int* param0 = &termMenu_nullParam_int
) {

    extend_containers(
        x_column-(menuTable.table.at(0).size()-1),
        y_row-(menuTable.table.size()-1)
    );
    if(container_insertKey(keyPress, x_column, y_row)==-1) return 1;
    menuTable.insertText(optName, x_column, y_row);


    optFunc_identifier.at(y_row).at(x_column) = 2;
    optFunc_id2_.at(y_row).at(x_column) = optFunc;
    optFunc_id2_p0.at(y_row).at(x_column) = param0;

    return 0;
}
/**
 * @brief add/set/edit an option/button info
 * @param optName `std::string` variable of name for the button
 * @param x_column x-coordinate/column location button/option to add/set/edit
 * @param y_row y-coordinate/row location of button/option to add/set/edit
 * @param keyPress int of the key event to react to: if `-1` then it will only be called with button press.
 * @param optFunc pointer to `void (char*)` function to call if that button/option is pressed/selected
 * @param param0 pointer to char var to pass to optFunc
 * @note - If `x_column`/`y_row` are further out than the already created dimensions (ex. there are less columns than x_column calls)
 * @note then it'll expand the 2d-container/table dimension along that axis.
 * @note - If `keyPress` isn't `-1` and that keyPress has already been "used" it'll set that keyPress to `-1` and return function -1
 * @return `0` if successfully added: `1` if an error occurred trying to addOpt
*/
int termMenu::addOpt(
    std::string optName,
    int x_column = 0,
    int y_row = 0,
    int keyPress = -1,
    TDEF_void_pChar optFunc = termMenu_nullFunc_void_pChar,
    char* param0 = &termMenu_nullParam_char
) {

    extend_containers(
        x_column-(menuTable.table.at(0).size()-1),
        y_row-(menuTable.table.size()-1)
    );
    if(container_insertKey(keyPress, x_column, y_row)==-1) return 1;
    menuTable.insertText(optName, x_column, y_row);


    optFunc_identifier.at(y_row).at(x_column) = 3;
    optFunc_id3_.at(y_row).at(x_column) = optFunc;
    optFunc_id3_p0.at(y_row).at(x_column) = param0;

    return 0;
}
/**
 * @brief add/set/edit an option/button info
 * @param optName `std::string` variable of name for the button
 * @param x_column x-coordinate/column location button/option to add/set/edit
 * @param y_row y-coordinate/row location of button/option to add/set/edit
 * @param keyPress int of the key event to react to: if `-1` then it will only be called with button press.
 * @param optFunc pointer to `void (std::string*)` function to call if that button/option is pressed/selected
 * @param param0 pointer to std::string var to pass to optFunc
 * @note - If `x_column`/`y_row` are further out than the already created dimensions (ex. there are less columns than x_column calls)
 * @note then it'll expand the 2d-container/table dimension along that axis.
 * @note - If `keyPress` isn't `-1` and that keyPress has already been "used" it'll set that keyPress to `-1` and return function -1
 * @return `0` if successfully added: `1` if an error occurred trying to addOpt
*/
int termMenu::addOpt(
    std::string optName,
    int x_column = 0,
    int y_row = 0,
    int keyPress = -1,
    TDEF_void_pStr optFunc = termMenu_nullFunc_void_pStr,
    std::string* param0 = &termMenu_nullParam_string
) {

    extend_containers(
        x_column-(menuTable.table.at(0).size()-1),
        y_row-(menuTable.table.size()-1)
    );
    if(container_insertKey(keyPress, x_column, y_row)==-1) return 1;
    menuTable.insertText(optName, x_column, y_row);


    optFunc_identifier.at(y_row).at(x_column) = 4;
    optFunc_id4_.at(y_row).at(x_column) = optFunc;
    optFunc_id4_p0.at(y_row).at(x_column) = param0;

    return 0;
}


/**
 * @brief rename the string of an option/button
 * @param x_column column location of option/button
 * @param y_row row location of option/button
 * @param new_optName `std::string` of new button name
 * @return whether it was successfull. `0`-success; `1`-error occured
*/
int termMenu::rename_opt(
    int x_column,
    int y_row,
    std::string new_optName
) {
    if(y_row>=menuTable.table.size() || x_column>=menuTable.table[0].size()) return 1;

    menuTable.insertText(new_optName, x_column, y_row);
    return 0;
}

/**
 * Main driver function for detecting keyboard events/presses and navigating the options/buttons
 * @param pos_x x coordinate on the terminal for where to display the menus from top-left corner
 * @param pos_y y coordinate on the terminal for where to display the menus from top-left corner
 * @param msDelay how many `milliseconds` to wait between each key-call/driver-loop-iteration
 * @param init_clear whether to clear the screen during driver function initialisation
 * @param loopFunc typedef `void ()` pointer to a void function to call during each driver loop iteration
 * @param iterClear whether to clear the screen during each loop iteration
 * @param highlight_textColour ansi code for colour of the text for the highlighted option
 * @param highlight_bgColour ansi code for colour of the background for the highlighted option
 * @param bg_textColour ansi code for colour of the text for the normal option
 * @param bg_bgColour ansi code for colour of the background for the normal option
 * @return pointer to a local static 2d int array {`static int pressed_option[2]`} with 2d coordinate location of pressed option/key if `callFromDriver`
 * was set to false during class initialisation.
*/
const int* termMenu::driver(
    int pos_x = 0,
    int pos_y = 0,
    int msDelay = 0,
    bool init_clear = true,
    TDEF_void__ loopFunc = termMenu_nullFunc_void__,
    bool iterClear = true,
    std::string highlight_textColour = "7",
    std::string highlight_bgColour = "7",
    std::string bg_textColour = "",
    std::string bg_bgColour = ""
) {

    if(pos_x==0) pos_x = 1;
    if(pos_y==0) pos_y = 1;

    termLoc[0] = pos_x;
    termLoc[1] = pos_y;

    getTermSize(termSize[0], termSize[1]);


    static int pressed_option[2] = {0, 0};
    bool loopInit = false; //boolean for whether the driver loop has ran a full iteration already
    int loopInit_count = 0;



    exitDriver = false;

    menuTable.strExport("\n", false, "\n", " ");


    // createTable displayTable = menuTable;

    if(display_dim[1][0]-display_dim[0][0]>menuTable.table[0].size()-1) {
        display_dim[1][0] = display_dim[0][0] + menuTable.table[0].size()-1;
    }
    if(display_dim[1][1]-display_dim[0][1]>menuTable.table.size()-1) {
        display_dim[1][1] = display_dim[0][1] + menuTable.table.size()-1;
    }


    if(cursorMode==-1) {
        display_dim[0][0] = 0;
        display_dim[0][1] = 0;
        display_dim[1][0] = menuTable.table[0].size()-1;
        display_dim[1][1] = menuTable.table.size()-1;
    }

    if(menuTable.getCellX(menuTable.table[0].size()-1,menuTable.table.size()-1)+pos_x>termSize[0]) { //lower right corner x check
        if(cursorMode==-1) cursorMode = 0;
        for(int x=menuTable.table[0].size()-1; x>=0; x--) {
            if(menuTable.getCellX(x,menuTable.table.size()-1)+pos_x<termSize[0]) {
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
    if(menuTable.getCellY(menuTable.table[0].size()-1,menuTable.table.size()-1)+pos_y>termSize[1]) { //lower right corner y check
        if(cursorMode==-1) cursorMode = 0;
        for(int y=menuTable.table.size()-1; y>=0; y--) {
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


    /// @brief bounding boxed table which will be displayed to terminal if cursorMode != -1
    std::vector<std::vector<std::string>> dispTable_vec(
        display_dim[1][1]-display_dim[0][1],
        std::vector<std::string>(
            display_dim[1][0]-display_dim[0][0],
            ""
        )
    );

    /// @brief `createTable` instance that is a cropped/ROI version of menuTable
    createTable dispTable(
        display_dim[1][0]-display_dim[0][0]+1,
        display_dim[1][1]-display_dim[0][1]+1
    );

    
    /// @brief width and height of displayed-window/dynamic-edges
    int dispDim[2] = {
        display_dim[1][0]-display_dim[0][0]+1,
        display_dim[1][1]-display_dim[0][1]+1
    };
    

    MEVENT mouse_event;

    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);



    int c = 0;
    if(!driverFuncInit) {
        if(cursorMode!=-1) {
            currCell[0] = dispDim[0]/2;
            currCell[1] = dispDim[1]/2;
        }
        else {
            bool xPlus=true;
            while(menuTable.table.at(currCell[1]).at(currCell[0])=="") {
                if(xPlus) currCell[0]++;
                else currCell[1]++;
                xPlus = !xPlus;
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
    

    bool driverInit = false;

    int init_driverCallKeys_count = init_driverCallKeys.size();
    int init_driverCallKeys_full = init_driverCallKeys_count;

    if(!driverFuncInit) driverFuncInit=true;
    while(/*(c=getch()) != 27 &&*/ !exitDriver) {
        std::this_thread::sleep_for(std::chrono::milliseconds(msDelay));


        if(init_driverCallKeys_count>0) {
            c = init_driverCallKeys.at(init_driverCallKeys_full-init_driverCallKeys_count);
            init_driverCallKeys_count--;
        }
        else c = getch();

        loopFunc();

        //???
        if(!callFunc && c==ERR && driverInit) continue;
        if(c==ERR && loopInit && loopInit_count==0) {
            driverInit = true;
            if(callFunc) continue;
        }

        if(loopInit_count>0) { loopInit_count--; }
        if(c==-1) c=0;

        lastKeyPress = c;

        if(c==KEY_LEFT) {
            currCell[0]--;
            if(currCell[0]<0) currCell[0] = menuTable.table[currCell[1]].size()-1;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="") { //skip empty cells
                for(int i=0; i<menuTable.table[currCell[1]].size(); i++) {
                    currCell[0]--;
                    if(currCell[0]<0) currCell[0] = menuTable.table[currCell[1]].size()-1;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
        else if(c==KEY_RIGHT) {
            currCell[0]++;
            if(currCell[0]>(menuTable.table[currCell[1]].size()-1)) currCell[0]=0;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="") {
                for(int i=0; i<menuTable.table[currCell[1]].size(); i++) {
                    currCell[0]++;
                    if(currCell[0]>(menuTable.table[currCell[1]].size()-1)) currCell[0]=0;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
        else if(c==KEY_UP) {
            currCell[1]--;
            if(currCell[1]<0) currCell[1] = menuTable.table.size()-1;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="") {
                for(int i=0; i<menuTable.table.size(); i++) {
                    currCell[1]--;
                    if(currCell[1]<0) currCell[1] = menuTable.table.size()-1;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
        else if(c==KEY_DOWN) {
            currCell[1]++;
            if(currCell[1]>(menuTable.table.size()-1)) currCell[1]=0;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="") {
                for(int i=0; i<menuTable.table.size(); i++) {
                    currCell[1]++;
                    if(currCell[1]>(menuTable.table.size()-1)) currCell[1]=0;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
        // else if(c==9) { // tab
        // }

        if(loopInit && (c==KEY_ENTER || c=='\n')) {
            if(callFunc) {
                customCall_argLess(currCell[0], currCell[1]);
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
            if(iterClear) std::cout << ansi_code+"2J";
        }

        bool subBreak = false;
        for(int y=0; y<option_key.size(); y++) {
            for(int x=0; x<option_key.at(y).size(); x++) {
                if(c==option_key.at(y).at(x)) {
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
                    subBreak = true;
                    break;
                    if(iterClear) std::cout << ansi_code+"2J";
                }
            }
            if(subBreak) break;
        }

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
        else if(cursorMode==0) {
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
            ANSI_mvprint(
                carriagePos[0]+pos_x,
                carriagePos[1]+pos_y,
                ansi_code+highlight_textColour+";"+highlight_bgColour+"m"+
                menuTable.table.at(currCell[1]).at(currCell[0])+
                std::string(button_totWidth*menuTable.maxColumnLen.at(currCell[0]), ' ')
                +ansi_code+bg_textColour+";"+bg_bgColour+"m"
            );

        }
        else if(cursorMode==1) {

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
            if(currCell[1]+dispDim[1]/2<=menuTable.table.size()-1) cursLock[1][1] = true;


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
                    display_dim[1][1] = menuTable.table.size()-1;
                    display_dim[0][1] = menuTable.table.size()-1 - dispDim[1];
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
        else if(cursorMode==2) {
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
                    else if(displayIDX_raw[y][x][1]>menuTable.table.size()-1) displayIDX_raw[y][x][1] -= menuTable.table.size();
                    
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
        refresh();

        if(!loopInit) loopInit=true;
    }
    loopInit = false;

    ANSI_mvprint(termSize[0]-10, 1, "exited");
    // std::cin.get();
    // std::cin.ignore();
    // std::cin.clear();

    endwin();


    return pressed_option;
}


/**
 * @brief get input from terminal
 * @param eventMethod the method in which to take input from the terminal.
 * `0` - default: `cbreak` and `noecho` is turned off and `std::cin.getline()` is used
 * `1` - getch(): it reads each keyinput one by one with the `getch()` function and displays the char's
 * @param pos_x terminal x/col position of where to place top left corner of the input box;
 * - `-1` means it'll place the box where the cursor is currently
 * @param pos_y terminal y/row position of where to place top left corner of the input box;
 * - `-1` means it'll place the box where the cursor is currently
 * @param bar_width width of the input box; `0` - dynamic width, same as char's input;
 *  - if width+pos_x exceeds terminal width it'll crop and adjust
 *  - if pos_x exceeds terminal width then it'll set it to `0`
 * @param bar_height height of the input box; `0` - dynamic width, changes if text goes beyond bar_width;
 *  - if height+pos_y exceeds terminal height it'll crop and adjust;
 *  - if pos_y exceeds terminal height then it'll set it to `0`
 * @param textLoc location/align of the input text. {`"left"`, `"center"`, `"right"`}
 * @param text_colour colour of the text
 * @param bg_colour colour of the background in the input box
 * @param startText text to print at the start of the input box
 * @param tabCalled pointer to a boolean to set as true if `tab` key was pressed
 * @return the inputted text received from the user
 * @note If `bar_width` is set to `0` and therefore not have a specified length, `bar_height` will be automatically
 * locked/set to `1`.
 * @note Pressing tab is equivalent to pressing enter.
*/
std::string termMenu::termInput(
    int eventMethod,
    int pos_x = -1,
    int pos_y = -1,
    int bar_width = 0,
    int bar_height = 1,
    std::string textAlign = "left",
    std::string text_colour = COLOUR_TEXT_WHITE,
    std::string bg_colour = COLOUR_BG_BLACK,
    std::string startText = "",
    bool *tabCalled = &termMenu_nullParam_bool
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
        nodelay(stdscr, FALSE);
        while((key_c=getch()) != '\n') {
            
            if(pos_x!=-1 && pos_y!=-1) {
                if(bar_height>0 && bar_width>0) {
                    for(int y=pos_y; y<pos_y+bar_height; y++) {
                        ANSI_mvprint(pos_x, y, ansi_code+text_colour+";"+bg_colour+"m"+std::string(bar_width, ' ') );
                    }
                }
            }

            // if(key_c=='\n') break;
            if(key_c==KEY_BACKSPACE && inputStr.length()>0) inputStr.erase(inputStr.length()-1);
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
        nodelay(stdscr, TRUE);
    }
    ANSI_mvprint(
        pos_x, pos_y,
        ansi_code+prev_textColour+";"+prev_bgColour+"m"
    );

    return inputStr;
}


int termMenu::redefine_function(int x_column, int y_row, TDEF_void__ newFunc) {
    if(x_column>=menuTable.table[0].size() || y_row>=menuTable.table.size()) return 1;
    optFunc_id0[x_column][y_row] = newFunc;

    return 0;
}

int termMenu::redefine_opt_id0_id1(int x_column, int y_row, TDEF_void_pBool optFunc, bool* param0) {
    if(x_column>=menuTable.table[0].size() || y_row>=menuTable.table.size()) return 1;
    
    optFunc_id0.at(y_row).at(x_column) = termMenu_nullFunc_void__;
    optFunc_id1_.at(y_row).at(x_column) = optFunc;
    optFunc_id1_p0.at(y_row).at(x_column) = param0;
    optFunc_identifier.at(y_row).at(x_column) = 1;

    return 0;
}

/**
 * get the X terminal position value (top left as 0,0) of the first char of desired createTable cell;
 * NOTE: `strExport` must be called as `exportStr` string var is what the position is relative to
 * @param column "x" parameter for cell
 * @param row "y" parameter for cell
 * @return the "x" position of the first character;
 * if `strExport` hasn't been called OR column/row is bigger than table dim,s then it'll return `-1`
*/
int termMenu::getCellPos_x(int column, int row, std::string rowSep="\n") {
    return menuTable.getCellX(column,row,rowSep);
}
/**
 * get the Y terminal position value (top left as 0,0) of the first char of desired cell;
 * NOTE: `strExport` must be called as `exportStr` string var is what the position is relative to
 * @param column "x" parameter for cell
 * @param row "y" parameter for cell
 * @return the "y" position of the first character;
 * if `strExport` hasn't been called OR column/row is bigger than table dim,s then it'll return `-1`
*/
int termMenu::getCellPos_y(int column, int row, std::string rowSep="\n") {
    return menuTable.getCellY(column,row,rowSep);
}

/// @brief get the string of cell name for a given cell/option
/// @param column x/column position ID to find name of
/// @param row y/row position ID to find name of
/// @return `std::string` of cellName
std::string termMenu::getCellName(int column, int row) {
    return menuTable.table.at(row).at(column);
}


/**
 * @brief add single keys to `init_driverCallKeys` vector
 * @param key integer to keyboard key signal
 * @param addLoc integer to element position for where to insert the key in the vector
*/
void termMenu::init_driverCallKeys_add(int key, int addLoc=-1) {
    if(addLoc<0 || addLoc>=init_driverCallKeys.size()) init_driverCallKeys.push_back(key);
    else {
        std::vector<int>::iterator itr = init_driverCallKeys.begin() + addLoc;
        init_driverCallKeys.insert(itr, key);
    }
}
/**
 * @brief add entire `std::vector<int>` to `init_driverCallKeys` vector
 * @param keys `std::vector<int>` to add onto container
 * @param addLoc integer to element position for where to insert the key in the vector
*/
void termMenu::init_driverCallKeys_add(std::vector<int> keys, int addLoc=-1) {
    if(addLoc<0 || addLoc>init_driverCallKeys.size()-1) addLoc = init_driverCallKeys.size();
    std::vector<int>::iterator itr = init_driverCallKeys.begin() + addLoc;
    init_driverCallKeys.insert(itr, keys.begin(), keys.end());
}
/// @brief clear the `init_driverCallKeys` container.
void termMenu::init_driverCallKeys_clear() {
    init_driverCallKeys.clear();
}


/**
 * @brief move the `currCell` location to a coordinate
 * @param col size_t column/x location
 * @param row size_t row/y location
*/
void termMenu::moveCurrCell(size_t col, size_t row) {

}
/**
 * @brief move the current `currCell` location by steps
 * @param xDir steps to move `currCell[0]` by; positive is to right side
 * @param yDir steps to move `currCell[1]` by; positive is down
*/
void termMenu::moveCurrCell(int xDir, int yDir) {

    if(xDir<0) {
        for(int x=xDir; x<0; x++) {
            currCell[0]--;
            if(currCell[0]<0) currCell[0] = menuTable.table[currCell[1]].size()-1;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="") { //skip empty cells
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
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="") {
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
            if(currCell[1]>(menuTable.table.size()-1)) currCell[1]=0;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="") {
                for(int i=0; i<menuTable.table.size(); i++) {
                    currCell[1]++;
                    if(currCell[1]>(menuTable.table.size()-1)) currCell[1]=0;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
    }
    else if(yDir<0) {
        for(int y=yDir; y<0; y++) {
            currCell[1]--;
            if(currCell[1]<0) currCell[1] = menuTable.table.size()-1;
            if(menuTable.table.at(currCell[1]).at(currCell[0])=="") {
                for(int i=0; i<menuTable.table.size(); i++) {
                    currCell[1]--;
                    if(currCell[1]<0) currCell[1] = menuTable.table.size()-1;
                    if(menuTable.table.at(currCell[1]).at(currCell[0])!="") break;
                }
            }
        }
    }

}

/// @brief get the total width of the menu that's displayed during `termMenu::driver`
/// @return integer of width characters that the terminal takes up in total
int termMenu::getMenu_width() {
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
/// @brief get the total height of the menu that's displayed during `termMenu::driver`
/// @return integer of height in characters that the terminal takes up in total
/// @note `termMenu::menuTable::strExport()` must have been called first
int termMenu::getMenu_height() {
    if(!menuTable.read_exportCalled()) {
        return menuTable.table.size();
    }
    return findOcc_char(menuTable.exportStr, '\n');
}


void clearScr() { std::cout<<"\x1B[2J"; std::cout.flush(); }
void bool_true(bool* toTrue) { (*toTrue)=true; }
void bool_false(bool* toFalse) { (*toFalse)=false; }
void bool_invert(bool* toInvert) { (*toInvert)=!(*toInvert); }


#endif
