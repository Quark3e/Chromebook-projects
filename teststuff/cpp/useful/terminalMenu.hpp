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
#if _WIN32
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif
#include <vector>
#include <algorithm>
#include <cmath>

#include "createTable.hpp"
#include "useful.hpp"



/// @brief placeholder function for empty function parameter
inline void termMenu_nullFunc_void__() {}
inline void termMenu_nullFunc_void_pBool(bool*) {}
inline void termMenu_nullFunc_void_pInt(int*) {}
inline void termMenu_nullFunc_void_pChar(char*) {}
inline void termMenu_nullFunc_void_pStr(std::string*) {}

inline void clearScr() { std::cout<<"\x1B[2J"; std::cout.flush(); }
inline void bool_true(bool* toTrue) { (*toTrue)=true; }
inline void bool_false(bool* toFalse) { (*toFalse)=false; }
inline void bool_invert(bool* toInvert) { (*toInvert)=!(*toInvert); }

extern bool         termMenu_nullParam_bool;
extern int          termMenu_nullParam_int;
extern char         termMenu_nullParam_char;
extern std::string  termMenu_nullParam_string;


class termMenu {

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
    bool exitDriver = false;

    /// @brief {x,y} coordinates of currently hovered cell in driver function.
    int currCell[2] = {0, 0};
    /// @brief Location of terminal
    int termLoc[2] = {0, 0};
    /// @brief width, height dimensions of the terminal
    int termSize[2] = {0, 0};
    /// @brief cell of the last pressed/selected.
    int pressedCell[2] = {0, 0};


    /**
     * @brief class constructor. Must be called before using class instances
     * @param callFromDriver whether to call a specific function during an option
     * selection or to return table "coordinate" 
     * @param menuColumns number of columns to initialise menu with
     * @param menuRows number of rows to initialise menu with
    */
    termMenu(bool callFromDriver, int menuColumns=1, int menuRows=1);
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
    std::string termInput(
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
    );

    /**
     * @brief set width of all buttons/options
     * @param width width of the button
     * @return 0 if successful; -1 if included width was too small (smaller than text length)
    */
    int setButtonWidth(int width);
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
    int setDisplayDim(int c0_x, int c0_y, int c1_x, int c1_y, int cursMode);

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
    int addOpt(std::string optName, int x_column=0, int y_row=0, int keyPress=-1, TDEF_void__ optFunc=termMenu_nullFunc_void__);
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
    int addOpt(std::string optName, int x_column=0, int y_row=0, int keyPress=-1, TDEF_void_pBool optFunc=termMenu_nullFunc_void_pBool, bool* param0=&termMenu_nullParam_bool);
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
    int addOpt(std::string optName, int x_column=0, int y_row=0, int keyPress=-1, TDEF_void_pInt optFunc=termMenu_nullFunc_void_pInt, int* param0=&termMenu_nullParam_int);
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
    int addOpt(std::string optName, int x_column=0, int y_row=0, int keyPress=-1, TDEF_void_pChar optFunc=termMenu_nullFunc_void_pChar, char* param0=&termMenu_nullParam_char);
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
    int addOpt(std::string optName, int x_column=0, int y_row=0, int keyPress=-1, TDEF_void_pStr optFunc=termMenu_nullFunc_void_pStr, std::string* param0=&termMenu_nullParam_string);


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
    const int* driver(
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
    );

    /**
     * @brief add single keys to `init_driverCallKeys` vector
     * @param key integer to keyboard key signal
     * @param addLoc integer to element position for where to insert the key in the vector
    */
    void init_driverCallKeys_add(int key, int addLoc=-1);
    /**
     * @brief add entire `std::vector<int>` to `init_driverCallKeys` vector
     * @param keys `std::vector<int>` to add onto container
     * @param addLoc integer to element position for where to insert the key in the vector
    */
    void init_driverCallKeys_add(std::vector<int> keys, int addLoc=-1);
    /// @brief clear the `init_driverCallKeys` container.
    void init_driverCallKeys_clear();
    /**
     * @brief move the `currCell` location to a coordinate
     * @param col size_t column/x location
     * @param row size_t row/y location
    */
    void moveCurrCell(size_t col, size_t row);
    /**
     * @brief move the current `currCell` location by steps
     * @param xDir steps to move `currCell[0]` by; positive is to right side
     * @param yDir steps to move `currCell[1]` by; positive is down
    */
    void moveCurrCell(int xDir, int yDir);

    int redefine_function(int x_column, int y_row, TDEF_void__ newFunc);
    int redefine_opt_id0_id1(int x_column, int y_row, TDEF_void_pBool optFunc, bool* param0);
    /**
     * @brief rename the string of an option/button
     * @param x_column column location of option/button
     * @param y_row row location of option/button
     * @param new_optName `std::string` of new button name
     * @return whether it was successfull. `0`-success; `1`-error occured
    */
    int rename_opt(int x_column, int y_row, std::string new_optName);
    /**
     * get the X terminal position value (top left as 0,0) of the first char of desired createTable cell;
     * NOTE: `strExport` must be called as `exportStr` string var is what the position is relative to
     * @param column "x" parameter for cell
     * @param row "y" parameter for cell
     * @return the "x" position of the first character;
     * if `strExport` hasn't been called OR column/row is bigger than table dim,s then it'll return `-1`
    */
    int getCellPos_x(int column, int row, std::string rowSep="\n");
    /**
     * get the Y terminal position value (top left as 0,0) of the first char of desired cell;
     * NOTE: `strExport` must be called as `exportStr` string var is what the position is relative to
     * @param column "x" parameter for cell
     * @param row "y" parameter for cell
     * @return the "y" position of the first character;
     * if `strExport` hasn't been called OR column/row is bigger than table dim,s then it'll return `-1`
    */
    int getCellPos_y(int column, int row, std::string rowSep="\n");
    /// @brief get the string of cell name for a given cell/option
    /// @param column x/column position ID to find name of
    /// @param row y/row position ID to find name of
    /// @return `std::string` of cellName
    std::string getCellName(int column, int row);

    /// @brief get the total width of the menu that's displayed during `termMenu::driver`
    /// @return integer of width characters that the terminal takes up in total
    int getMenu_width();
    /// @brief get the total height of the menu that's displayed during `termMenu::driver`
    /// @return integer of height in characters that the terminal takes up in total
    /// @note `termMenu::menuTable::strExport()` must have been called first
    int getMenu_height();
};


#endif
