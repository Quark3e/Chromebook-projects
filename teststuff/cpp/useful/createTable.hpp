
#pragma once
#ifndef HPP_CREATETABLE
#define HPP_CREATETABLE

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>

// #include "useful.hpp"


class createTable {

    /// @brief `{width, height}` of table "window"
    int table_dim[2];

    // /// @brief tables number of rows and columns: `{rows, columns}`
    // int table_rowcol[2] = {0, 0};

    void updateTableInfo();

    /// @brief whether to use column labels
    bool use_label_column = false;

    bool exportCalled = false;
    bool export_useBorder = false;


    std::string columnSep = "";
    std::string rowSep = "";
    char border_widthChar   = '-';
    char border_heightChar  = '|';

    /**
     * 2d dimensions of a cell {width, height} for each cell
    */
    std::vector<std::vector<int>> cellDim[2];

    /**
     * number of rows that tableRow contains
    */
    std::vector<int> rowRows;

    public:
    /// @brief length of longest string in that column
    std::vector<int> maxColumnLen;
    /// @brief extra padding beside maxColumnLen (number of space to achieve maxColumnLen)
    std::vector<int> columnPadding;
    /// @brief label for each column
    std::vector<std::string> columnLabel;

    /**
     * @brief table string vector: `[row][colum]`
     * [
     *  [0, 0, 0, 0],
     *  [0, 0, 0, 0]
     * ]
     * */
    std::vector<std::vector<std::string>> table;
    /**
     * type of data type in that cell.
     * options: {`"text"`, `"number"`}
    */
    std::vector<std::vector<std::string>> cellType;


    /// @brief complete string that's generated with `strExport()`
    std::string exportStr;

    createTable();
    /**
     * @brief table constructor
     * @param columns number of columns to construct class object table with.
     * @param rows number of rows to construct class object table with.
    */
    createTable(int columns, int rows);

    /// @brief find number of char occurances in a string
    /// @param toCheck string to find occurrences in
    /// @param toFind char to find occurrences
    /// @return number of occurrences
    int findOcc_char(std::string toCheck, char toFind);

    /**
     * @brief add rows to `table`
     * @param numRows number of rows to add to the table
     * @param defaultStr default string value to place on the new row cells
    */
    void add_row(int numRows=1, std::string defaultStr="");
    /**
     * @brief add columns to `table`
     * @param numCols number of columns to add to the table
     * @param defaultStr default string value to place on the new column cells
    */
    void add_col(int numCols=1, std::string defaultStr="");
    /**
     * @brief add string to the end of a cell string. Can be used to add prefixes to numbers
     * @param text string text to add at the end of cell string
     * @param column column index
     * @param row row index
     * @param loc where to add the text: `0`-rightside/after-existing; `1`-leftside/before-existing 
    */
    void add_to_cell(std::string text, int column, int row, int loc=0);
    /**
     * @brief set the cell string of `table`
     * @param newText string to place in cell `table[{row}][{column}]`
     * @param column column index
     * @param row row index
    */
    void insertText(std::string newText, int column, int row);
    /**
     * @brief Insert int number onto a cell in `table`.
     * @param num integer number to insert.
     * @param column index to which column to insert number in.
     * @param row index to which row to insert number in.
    */
    void insertNum(int num, int column, int row=0);
    /**
     * @brief Insert float number onto a cell in `table`.
     * @param num float number to insert.
     * @param column index to which column to insert number in.
     * @param row index to which row to insert number in.
     * @param precision decimal precision for how many decimal numbers to use in string.
    */
    void insertNum(float num, int column, int row=0, int precision=2);

    // int copyFrom(createTable copyFrom, int TL_insert_x, int TL_insert_y, int c0_x, int c0_y, int c1_x, int c1_y, std::string prioRemove);
    
    /**
     * Copy settings such as `colSep`, padding, e.t.c. from a different `createTable` instance to this one
     * @param copyFrom a `createTable` object to save settings from
     * @return whether it was successful or not: `0` - successfully copies settings; `-1` - unsuccessful/error in copying
    */
    int copySettings(createTable copyFrom);
    int copyTo_settings(createTable& copyTo);

    /**
     * @brief set extra column padding to each cell in each column according to 
     * that columns longest string + this funcions input padding
     * @param paddings `std::vector<int>` of new padding values
    */
    void setColPadding(std::vector<int> paddings);
    /**
     * @brief Set the width of the column
     * 
     * @param column column index to set the width of
     * @param width number of characters to set the column width to
     */
    void setColWidth(int column, int width);

    /**
     * @brief check and update `maxColumnLen` vector value accuracy with table vector
     * @param whichRow which row to compare maxColumnLen with. If it's `-1`
     * then it'll compare the vector with every row
    */
    void checkMaxLen(int whichRow=-1);

    /**
     * get the Y terminal position value (top left as 0,0) of the first char of desired cell;
     * NOTE: `strExport` must be called as `exportStr` string var is what the position is relative to
     * @param column "x" parameter for cell
     * @param row "y" parameter for cell
     * @return the "y" position of the first character;
     * if `strExport` hasn't been called OR column/row is bigger than table dim,s then it'll return `-1`
    */
    int getCellY(int column, int row, std::string rowSep="\n");
    /**
     * get the X terminal position value (top left as 0,0) of the first char of desired cell;
     * NOTE: `strExport` must be called as `exportStr` string var is what the position is relative to
     * @param column "x" parameter for cell
     * @param row "y" parameter for cell
     * @return the "x" position of the first character;
     * if `strExport` hasn't been called OR column/row is bigger than table dim,s then it'll return `-1`
    */
    int getCellX(int column, int row, std::string rowSep="\n");
    /**
     * get the {X, Y} terminal position value (top left as 0,0) of the first char of desired cell;
     * @attention `strExport` must be called as `exportStr` string var is what the position is relative to
     * @param column "x" parameter for cell
     * @param row "y" parameter for cell
     * @return pointer to a static int array of cell pos of the first character;
     * if `strExport` hasn't been called OR column/row is bigger than table dim,s then it'll return `-1` in either/both element
    */
    const int* getCellPos(int column, int row, std::string rowSep="\n");

    /// @brief Get longest column text width of all columns
    /// @return returns the length of the longest column text
    int getColTotMax();
    /// @brief export/finalise a complete string of the complete table
    /// @param startRow a string before the table.
    /// @param createBorder whether to create an outer border around the table(does not include `startRow`)
    /// @param rowEnd row separator string
    /// @param colSep column separator string
    /// @return full string, same as `exportStr`
    std::string strExport(
        std::string startRow = "",
        bool createBorder = true,
        std::string rowEnd = "\n",
        std::string colSep = "|"
    );
    /// @brief use the class's isolatedExport same as `createTable::strExport` but with a separate external 2d container table variable.
    /// @param extTable the external table to run the export function on
    /// @param startRow a string before the table.
    /// @param createBorder whether to create an outer border around the table(does not include `startRow`)
    /// @param rowEnd row separator string
    /// @param colSep column separator string
    /// @return the full exported string
    std::string isolatedExport(
        std::vector<std::vector<std::string>> extTable,
        std::string startRow = "",
        bool createBorder = true,
        std::string rowEnd = "\n",
        std::string colSep = "|"
    );

    bool read_exportCalled() { return exportCalled; }

};


#endif
