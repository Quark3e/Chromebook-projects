
#pragma once
#ifndef HPP_CREATETABLE
#define HPP_CREATETABLE

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "useful.hpp"


class createTable {

    /// @brief `{width, height}` of table "window"
    int table_dim[2];

    // /// @brief tables number of rows and columns: `{rows, columns}`
    // int table_rowcol[2] = {0, 0};

    void updateTableInfo();
    void checkMaxLen(int whichRow);

    bool use_label_column = false;

    public:
    std::vector<int> maxColumnLen;
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
    std::string exportStr;

    createTable();
    createTable(int columns, int rows);

    void add_row(int numRows, std::string defaultStr);
    void add_col(int numCols, std::string defaultStr);

    void add_to_cell(std::string text, int column, int row);

    void insertText(std::string newText, int column, int row, int cellWidth);
    void insertNum(int num, int column, int row, int cellWidth);
    void insertNum(float num, int column, int row, int cellWidth, int precision);

    std::string strExport(
        std::string startRow,
        bool createBorder,
        std::string rowEnd,
        std::string colSep
    );
};

void createTable::updateTableInfo() {}

/**
 * @brief check `maxColumnLen` vectors accuracy with table vector
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
    table       = std::vector<std::vector<std::string>>(rows, std::vector<std::string>(columns, ""));
    cellType    = std::vector<std::vector<std::string>>(rows, std::vector<std::string>(columns, "text"));
    columnLabel = std::vector<std::string>(columns, "");
    maxColumnLen= std::vector<int>(columns, 0);
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
    for(size_t row=0; row<table.size(); row++) {
        for(int n=0; n<numCols; n++) {
            table[row].push_back(defaultStr);
            cellType[row].push_back("text");
        }
    }

    for(int n=0; n<numCols; n++) {
        maxColumnLen.push_back(0);
        columnLabel.push_back("");
    }
    // table_rowcol[1] += numCols;
}

/**
 * @brief add string to the end of a cell string. Can be used to add prefixes to numbers
 * @param text string text to add at the end of cell string
 * @param column column index
 * @param row row index
*/
void createTable::add_to_cell(std::string text, int column, int row) {
    table[row][column] += text;
}

/**
 * @brief set the cell string of `table`
 * @param newText string to place in cell `table[{row}][{column}]`
 * @param column column index
 * @param row row index
 * @param cellWidth minimum number of characters to fill cell with
*/
void createTable::insertText(std::string newText, int column, int row, int cellWidth=0) {
    std::stringstream ss;
    ss << std::left << std::fixed << std::setw(cellWidth) << newText;
    table[row][column] = ss.str();
    cellType[row][column] = "text";
}
/**
 * @brief Insert int number onto a cell in `table`.
 * @param num integer number to insert.
 * @param column index to which column to insert number in.
 * @param row index to which row to insert number in.
 * @param cellWidth minimum number of characters to fill cell with
*/
void createTable::insertNum(int num, int column, int row, int cellWidth=0) {
    std::stringstream ss;
    ss << std::right << std::fixed << std::setw(cellWidth) << num;
    table[row][column] = ss.str();
    cellType[row][column] = "number";
}

/**
 * @brief Insert float number onto a cell in `table`.
 * @param num float number to insert.
 * @param column index to which column to insert number in.
 * @param row index to which row to insert number in.
 * @param cellWidth minimum number of characters to fill cell with
 * @param precision decimal precision for how many decimal numbers to use in string.
*/
void createTable::insertNum(float num, int column, int row, int cellWidth=0, int precision=2) {
    std::stringstream ss;
    ss << std::right << std::fixed << std::setw(cellWidth) << std::setprecision(precision) << num;
    table[row][column] = ss.str();
    cellType[row][column] = "number";
}


std::string createTable::strExport(
    std::string startRow = "",
    bool createBorder = true,
    std::string rowEnd = "\n",
    std::string colSep = "|"
) {
    checkMaxLen();
    stringstream ss;
    exportStr = "";
    table_dim[0] = 0;
    table_dim[1] = static_cast<int>(table.size());
    for(int& var: maxColumnLen) table_dim[0]+=var;

    table_dim[0] += (static_cast<int>(table[0].size()) - 1)*colSep.length();
    if(createBorder) {
        table_dim[0]+=2;
        table_dim[1]+=2;
        exportStr += std::string(table_dim[0], '-') + rowEnd;
    }


    for(size_t row=0; row<table.size(); row++) {
        if(createBorder) exportStr += "|";
        for(size_t col=0; col<table[row].size(); col++) {
            // if(cellType[row][col]=="text") exportStr += table[row][col];
            // else if(cellType[row][col]=="number") {}
            exportStr += table[row][col];
            if(col<table[row].size()-1) exportStr += colSep;
        }
        if(createBorder) exportStr += "|";
        exportStr += rowEnd;
    }
    if(createBorder) {
        exportStr += std::string(table_dim[0], '-') + rowEnd;
    }

    return exportStr;
}



#endif