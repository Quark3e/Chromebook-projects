
#pragma once
#ifndef HPP_CREATETABLE
#define HPP_CREATETABLE

#include <iostream>
#include <string>
#include <vector>

#include "useful.hpp"


class createTable {

    /// @brief `{width, height}` of table "window"
    int table_dim[2];

    public:
    bool create_border = true;

    std::vector<std::vector<std::string>> table;

    createTable();
    createTable(int columns, int rows);

    void add_row(string label, int columns);
};


/// @brief empty default constructor
createTable::createTable() {}

createTable::createTable(
    int columns,
    int rows
) {
    table_dim[0] = columns;
    table_dim[1] = rows;
    table = std::vector<std::vector<std::string>>(rows, std::vector<std::string>(columns));
}

void createTable::add_row(string label, int columns) {
    if(columns<2) {
        std::cout << "ERROR: createTable::add_row(): too few columns." << std::endl;
        return;
    }
    table.push_back(std::vector<std::string>(columns));
    table.back()[0] = label;
}



#endif