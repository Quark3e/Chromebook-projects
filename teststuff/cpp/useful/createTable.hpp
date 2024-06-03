
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
    createTable(int columns, int rows);

    int findOcc_char(std::string toCheck, char toFind);


    void add_row(int numRows, std::string defaultStr);
    void add_col(int numCols, std::string defaultStr);

    void add_to_cell(std::string text, int column, int row, int loc);

    void insertText(std::string newText, int column, int row);
    void insertNum(int num, int column, int row);
    void insertNum(float num, int column, int row, int precision);

    // int copyFrom(createTable copyFrom, int TL_insert_x, int TL_insert_y, int c0_x, int c0_y, int c1_x, int c1_y, std::string prioRemove);
    int copySettings(createTable copyFrom);
    int copyTo_settings(createTable& copyTo);


    void setColPadding(std::vector<int> paddings);
    
    void checkMaxLen(int whichRow);

    int getCellY(int column, int row, std::string rowSep);
    int getCellX(int column, int row, std::string rowSep);
    const int* getCellPos(int column, int row, std::string rowSep);

    int getColTotMax();

    std::string strExport(
        std::string startRow,
        bool createBorder,
        std::string rowEnd,
        std::string colSep
    );
    std::string isolatedExport(
        std::vector<std::vector<std::string>> extTable,
        std::string startRow,
        bool createBorder,
        std::string rowEnd,
        std::string colSep
    );

    bool read_exportCalled() { return exportCalled; }

};

void createTable::updateTableInfo() {}

/**
 * @brief set extra column padding to each cell in each column according to 
 * that columns longest string + this funcions input padding
 * @param paddings `std::vector<int>` of new padding values
*/
void createTable::setColPadding(std::vector<int> paddings) {
    if(paddings.size() != columnPadding.size()) {
        std::cout << "\nERROR: createTable::setColPadding: input padding vector is not same size as createTable.columnPadding var size."<<std::endl;
        return;
    }
    for(size_t i=0; i<columnPadding.size(); i++) {
        columnPadding[i] = paddings[i];
    }
}

/**
 * @brief check and update `maxColumnLen` vector value accuracy with table vector
 * @param whichRow which row to compare maxColumnLen with. If it's `-1`
 * then it'll compare the vector with every row
*/
void createTable::checkMaxLen(int whichRow=-1) {
    for(size_t i=0; i<table[0].size(); i++) { //iterate columns
        if(whichRow==-1) {
            for(size_t row=0; row<table.size(); row++) { //iterate rows
                if(table[row][i].length() > maxColumnLen[i]) {
                    maxColumnLen[i] = table[row][i].length();
                }
            }
        }
        else {
            if(table[whichRow][i].length() > maxColumnLen[i]) {
                maxColumnLen[i] = table[whichRow][i].length();
            }
        }
    }

}

/// @brief empty default constructor
createTable::createTable() {}
/**
 * @brief table constructor
 * @param columns number of columns to construct class object table with.
 * @param rows number of rows to construct class object table with.
*/
createTable::createTable(
    int columns,
    int rows
) {
    table           = std::vector<std::vector<std::string>>(rows, std::vector<std::string>(columns, ""));
    cellType        = std::vector<std::vector<std::string>>(rows, std::vector<std::string>(columns, "text"));
    columnLabel     = std::vector<std::string>(columns, "");
    columnPadding   = std::vector<int>(columns, 1);
    maxColumnLen    = std::vector<int>(columns, 0);
}

/**
 * @brief add rows to `table`
 * @param numRows number of rows to add to the table
 * @param defaultStr default string value to place on the new row cells
*/
void createTable::add_row(int numRows=1, std::string defaultStr="") {
    for(int i=0; i<numRows; i++) {
        table.push_back(std::vector<std::string>(table[0].size(), defaultStr));
        cellType.push_back(std::vector<std::string>(table[0].size(), "text"));
    }
    // table_rowcol[0] += numRows;
}

/**
 * @brief add columns to `table`
 * @param numCols number of columns to add to the table
 * @param defaultStr default string value to place on the new column cells
*/
void createTable::add_col(int numCols=1, std::string defaultStr="") {
    for(size_t row=0; row<table.size(); row++) { //iterate through each row
        for(int n=0; n<numCols; n++) {
            table[row].push_back(defaultStr);
            cellType[row].push_back("text");
        }
    }

    for(int n=0; n<numCols; n++) { //
        maxColumnLen.push_back(0);
        columnPadding.push_back(1);
        columnLabel.push_back("");
    }
    // table_rowcol[1] += numCols;
}

/**
 * @brief add string to the end of a cell string. Can be used to add prefixes to numbers
 * @param text string text to add at the end of cell string
 * @param column column index
 * @param row row index
 * @param loc where to add the text: `0`-rightside/after-existing; `1`-leftside/before-existing 
*/
void createTable::add_to_cell(std::string text, int column, int row, int loc=0) {
    if(loc==0) table[row][column] += text;
    else if(loc==1) table[row][column] = text + table[row][column];
}

/**
 * @brief set the cell string of `table`
 * @param newText string to place in cell `table[{row}][{column}]`
 * @param column column index
 * @param row row index
*/
void createTable::insertText(std::string newText, int column, int row) {
    if(column>=table[0].size() || row>=table.size()) return;
    table[row][column] = newText;
    cellType[row][column] = "text";
}

/**
 * @brief Insert int number onto a cell in `table`.
 * @param num integer number to insert.
 * @param column index to which column to insert number in.
 * @param row index to which row to insert number in.
*/
void createTable::insertNum(int num, int column, int row=0) {
    if(column>=table[0].size() || row>=table.size()) return;
    table[row][column] = std::to_string(num);
    cellType[row][column] = "number";
}

/**
 * @brief Insert float number onto a cell in `table`.
 * @param num float number to insert.
 * @param column index to which column to insert number in.
 * @param row index to which row to insert number in.
 * @param precision decimal precision for how many decimal numbers to use in string.
*/
void createTable::insertNum(float num, int column, int row=0, int precision=2) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << num;
    table[row][column] = ss.str();
    cellType[row][column] = "number";
}


/// @brief export/finalise a complete string of the complete table
/// @param startRow a string before the table.
/// @param createBorder whether to create an outer border around the table(does not include `startRow`)
/// @param rowEnd row separator string
/// @param colSep column separator string
/// @return full string, same as `exportStr`
std::string createTable::strExport(
    std::string startRow = "",
    bool createBorder = true,
    std::string rowEnd = "\n",
    std::string colSep = "|"
) {
    export_useBorder = createBorder;
    columnSep = colSep;
    rowSep = rowEnd;

    exportCalled = true;
    checkMaxLen();
    std::stringstream ss;
    exportStr = startRow;
    table_dim[0] = 0;
    table_dim[1] = static_cast<int>(table.size());
    for(size_t i=0; i<maxColumnLen.size(); i++) {
        table_dim[0]+=maxColumnLen[i]+columnPadding[i];
    }

    table_dim[0] += (static_cast<int>(table[0].size()) - 1)*colSep.length();
    if(createBorder) {
        table_dim[0]+=2;
        table_dim[1]+=2;
        exportStr += std::string(table_dim[0], border_widthChar) + rowEnd;
    }

    cellDim[0] = std::vector<std::vector<int>>(table.size(), std::vector<int>(table[0].size(), 0));
    cellDim[1] = std::vector<std::vector<int>>(table.size(), std::vector<int>(table[0].size(), 0));
    rowRows = std::vector<int>(table.size(), 1);

    for(size_t row=0; row<table.size(); row++) {
        int count_newline=0;
        for(size_t col=0; col<table[row].size(); col++) {
            int temp = findOcc_char(table[row][col], '\n');
            if(temp>count_newline) count_newline = temp;
            cellDim[0][row][col] = table[row][col].length();
            cellDim[1][row][col] = temp+1;
        }
        table_dim[1]+=count_newline;
        rowRows[row] = count_newline+1;

        std::vector<std::string> extraRows;
        if(count_newline>0) extraRows = std::vector<std::string>(count_newline, "");

        if(createBorder) {
            exportStr += border_heightChar;
            if(count_newline>0) {
                for(size_t i=0; i<extraRows.size(); i++) extraRows[i]+=border_heightChar;
            }
        }
        for(size_t col=0; col<table[row].size(); col++) {
            ss.str(std::string());
            ss.clear();
            if(cellType[row][col]=="text") ss<<std::left;
            else if(cellType[row][col]=="number") ss<<std::right;

            if(count_newline>0) {
                size_t tPos=table[row][col].find('\n', 0)+1, ePos=0, nCount=0;
                std::stringstream sss;
                while(table[row][col].find('\n', tPos)!=std::string::npos) {
                    sss.str(std::string());
                    sss.clear();
                    if(cellType[row][col]=="text") sss<<std::left;
                    else if(cellType[row][col]=="number") sss<<std::right;
                    // size_t newline_endPos = table[row][col].find('\n',tPos);
                    ePos = table[row][col].find('\n', tPos);
                    sss<<std::setw(maxColumnLen[col]+columnPadding[col])<<table[row][col].substr(
                        tPos,
                        ePos-(tPos)
                    );

                    tPos = ePos+1;
                    extraRows[nCount] += sss.str();
                    if(col<table[row].size()-1) extraRows[nCount] += colSep;
                    nCount++;
                }
                sss.str(std::string());
                sss.clear();
                
                if(cellType[row][col]=="text") sss<<std::left;
                else if(cellType[row][col]=="number") sss<<std::right;
                // size_t newline_endPos = table[row][col].find('\n',tPos);
                ePos = table[row][col].find('\n', tPos);
                sss<<std::setw(maxColumnLen[col]+columnPadding[col])<<table[row][col].substr(
                    tPos,
                    ePos-(tPos)
                );

                tPos = ePos+1;
                extraRows[nCount] += sss.str();
                if(col<table[row].size()-1) extraRows[nCount] += colSep;

            }
            ss<<std::setw(maxColumnLen[col]+columnPadding[col])<<table[row][col].substr(
                0, table[row][col].find('\n',0)
            );
            exportStr += ss.str();
            if(col<table[row].size()-1) exportStr += colSep;
        }
        if(createBorder) { exportStr += border_heightChar; }
        exportStr += rowEnd;
        if(count_newline>0) {
            for(size_t i=0; i<extraRows.size(); i++) {
                if(createBorder) extraRows[i]+=border_heightChar;
                extraRows[i] += rowEnd;
                exportStr += extraRows[i];
            }
        }
    }
    if(createBorder) {
        exportStr += std::string(table_dim[0], border_widthChar) + rowEnd;
    }

    return exportStr;
}

/// @brief use the class's isolatedExport same as `createTable::strExport` but with a separate external 2d container table variable.
/// @param extTable the external table to run the export function on
/// @param startRow a string before the table.
/// @param createBorder whether to create an outer border around the table(does not include `startRow`)
/// @param rowEnd row separator string
/// @param colSep column separator string
/// @return the full exported string
std::string createTable::isolatedExport(
    std::vector<std::vector<std::string>> extTable,
    std::string startRow = "",
    bool createBorder = true,
    std::string rowEnd = "\n",
    std::string colSep = "|"
) {
    std::string returStr = "";
    std::stringstream ss;
    returStr = startRow;
    table_dim[0] = 0;
    table_dim[1] = static_cast<int>(extTable.size());
    for(size_t i=0; i<maxColumnLen.size(); i++) {
        table_dim[0]+=maxColumnLen[i]+columnPadding[i];
    }

    table_dim[0] += (static_cast<int>(extTable[0].size()) - 1)*colSep.length();
    if(createBorder) {
        table_dim[0]+=2;
        table_dim[1]+=2;
        returStr += std::string(table_dim[0], border_widthChar) + rowEnd;
    }


    for(size_t row=0; row<extTable.size(); row++) {
        if(createBorder) returStr += border_heightChar;
        for(size_t col=0; col<extTable[row].size(); col++) {
            ss.str(std::string());
            ss.clear();
            if(cellType[row][col]=="text") ss<<std::left;
            else if(cellType[row][col]=="number") ss<<std::right;
            ss<<std::setw(maxColumnLen[col]+columnPadding[col])<<extTable[row][col];
            returStr += ss.str();
            if(col<extTable[row].size()-1) returStr += colSep;
        }
        if(createBorder) returStr += border_heightChar;
        returStr += rowEnd;
    }
    if(createBorder) {
        returStr += std::string(table_dim[0], border_widthChar) + rowEnd;
    }

    return returStr;
}

/**
 * get the X terminal position value (top left as 0,0) of the first char of desired cell;
 * NOTE: `strExport` must be called as `exportStr` string var is what the position is relative to
 * @param column "x" parameter for cell
 * @param row "y" parameter for cell
 * @return the "x" position of the first character;
 * if `strExport` hasn't been called OR column/row is bigger than table dim,s then it'll return `-1`
*/
int createTable::getCellX(
    int column,
    int row,
    std::string rowSep="\n"
) {
    if(!exportCalled) return -1;
    if(row>=table.size() || column>=table[row].size()) return -1;
    int resultX=0;

    if(export_useBorder) resultX+=1;
    for(int i=0; i<column; i++) {
        resultX+=maxColumnLen.at(i)+columnSep.length()+columnPadding.at(i);
    }
    return resultX;
}
/**
 * get the Y terminal position value (top left as 0,0) of the first char of desired cell;
 * NOTE: `strExport` must be called as `exportStr` string var is what the position is relative to
 * @param column "x" parameter for cell
 * @param row "y" parameter for cell
 * @return the "y" position of the first character;
 * if `strExport` hasn't been called OR column/row is bigger than table dim,s then it'll return `-1`
*/
int createTable::getCellY(
    int column,
    int row,
    std::string rowSep="\n"
) {
    if(!exportCalled) return -1;
    if(row>=table.size() || column>=table[row].size()) return -1;

    int resultY=1;
    for(size_t i=0; i<row; i++) {
        resultY+=rowRows[i];
    }
    if(export_useBorder) resultY+=1;


    return resultY;
}
/**
 * get the {X, Y} terminal position value (top left as 0,0) of the first char of desired cell;
 * @attention `strExport` must be called as `exportStr` string var is what the position is relative to
 * @param column "x" parameter for cell
 * @param row "y" parameter for cell
 * @return pointer to a static int array of cell pos of the first character;
 * if `strExport` hasn't been called OR column/row is bigger than table dim,s then it'll return `-1` in either/both element
*/
const int* createTable::getCellPos(
    int column,
    int row,
    std::string rowSep="\n"
) {
    static int pos[2] = {0, 0};
    pos[0] = getCellX(column, row, rowSep);
    pos[1] = getCellY(column, row, rowSep);

    return pos;
}

/// @brief Get longest column text width of all columns
/// @return returns the length of the longest column text
int createTable::getColTotMax() {
    double max = *std::max_element(maxColumnLen.begin(), maxColumnLen.end());
    return static_cast<int>(max);
}


// /**
//  * copy table info such as names, column padding, special symbols e.t.c from one `createTable` object to another
//  * @param copyFrom `createTable` object to copy *from*
//  * @param TL_insert_x x pos on where to insert in this from the top left corner of copyFrom
//  * @param TL_insert_y y pos on where to insert in this from the top left corner of copyFrom
//  * @param c0_x
//  * @param c0_y
//  * @param c1_x
//  * @param c1_y
//  * @param prioRemove side to prioritise cutting/cropping/removing if `toCopy` object dimensions is larger
//  * @return whether the copying was successful or not
// */
// int createTable::copyFrom(
//     createTable copyFrom,
//     int TL_insert_x,
//     int TL_insert y,
//     int c0_x = 0,
//     int c0_y = 0,
//     int c1_x = -1,
//     int c1_y = -1,
//     std::string prioRemove = "BR"
// ) {
// }

/**
 * Copy settings such as `colSep`, padding, e.t.c. from a different `createTable` instance to this one
 * @param copyFrom a `createTable` object to save settings from
 * @return whether it was successful or not: `0` - successfully copies settings; `-1` - unsuccessful/error in copying
*/
int createTable::copySettings(
    createTable copyFrom
) {

}

int createTable::copyTo_settings(
    createTable& copyTo
) {
    copyTo.use_label_column = use_label_column;
    copyTo.columnSep = columnSep;
    copyTo.rowSep = rowSep;
    copyTo.border_widthChar = border_widthChar;
    copyTo.border_heightChar = border_heightChar;
}


/// @brief find number of char occurances in a string
/// @param toCheck string to find occurrences in
/// @param toFind char to find occurrences
/// @return number of occurrences
int createTable::findOcc_char(std::string toCheck, char toFind) {
    std::string::difference_type n = std::count(toCheck.begin(), toCheck.end(), toFind);
    return static_cast<int>(n);
}


#endif
