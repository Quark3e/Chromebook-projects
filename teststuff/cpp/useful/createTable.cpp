
#include "createTable.hpp"



void createTable::updateTableInfo() {}

void createTable::setColPadding(std::vector<int> paddings) {
    if(paddings.size() != columnPadding.size()) {
        std::cout << "\nERROR: createTable::setColPadding: input padding vector is not same size as createTable.columnPadding var size."<<std::endl;
        return;
    }
    for(size_t i=0; i<columnPadding.size(); i++) {
        columnPadding[i] = paddings[i];
    }
}

void createTable::checkMaxLen(int whichRow) {
    // maxColumnLen = std::vector<int>(table[0].size(), 0);
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

void createTable::setColWidth(int column, int width) {
    if(column>=table[0].size()) throw std::invalid_argument("createTable::setColWidth: column index out of range.");
    if(column<0) {
        column = table[0].size() + column;
    }
    maxColumnLen[column] = width;
}

createTable::createTable() {}
createTable::createTable(int columns, int rows) {
    table           = std::vector<std::vector<std::string>>(rows, std::vector<std::string>(columns, ""));
    cellType        = std::vector<std::vector<std::string>>(rows, std::vector<std::string>(columns, "text"));
    columnLabel     = std::vector<std::string>(columns, "");
    columnPadding   = std::vector<int>(columns, 1);
    maxColumnLen    = std::vector<int>(columns, 0);
}

void createTable::add_row(int numRows, std::string defaultStr) {
    for(int i=0; i<numRows; i++) {
        table.push_back(std::vector<std::string>(table[0].size(), defaultStr));
        cellType.push_back(std::vector<std::string>(table[0].size(), "text"));
    }
    // table_rowcol[0] += numRows;
}

void createTable::add_col(int numCols, std::string defaultStr) {
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


void createTable::add_to_cell(std::string text, int column, int row, int loc) {
    if(loc==0) table[row][column] += text;
    else if(loc==1) table[row][column] = text + table[row][column];
}

void createTable::insertText(std::string newText, int column, int row) {
    if(column>=table[0].size()) throw std::runtime_error("createTable::insertText(): column out of range.");
    if(row>=table.size())       throw std::runtime_error("createTable::insertText(): row out of range.");
    table[row][column] = newText;
    cellType[row][column] = "text";
}

void createTable::insertNum(int num, int column, int row) {
    if(column>=table[0].size()) throw std::runtime_error("createTable::insertNum(): column out of range.");
    if(row>=table.size())       throw std::runtime_error("createTable::insertNum(): row out of range.");
    table[row][column] = std::to_string(num);
    cellType[row][column] = "number";
}

void createTable::insertNum(float num, int column, int row, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << num;
    table[row][column] = ss.str();
    cellType[row][column] = "number";
}

std::string createTable::strExport(
    std::string startRow,
    bool createBorder,
    std::string rowEnd,
    std::string colSep
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
                    if(col<table[row].size()-1) {
                        if(col+1<table[row].size() && maxColumnLen[col+1]>0) extraRows[nCount] += colSep;
                        else {
                            extraRows[nCount] += std::string(colSep.size(), ' ');
                        }
                    }
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
                if(col<table[row].size()-1) {
                    if(col+1<table[row].size() && maxColumnLen[col+1]>0) extraRows[nCount] += colSep;
                    else {
                        extraRows[nCount] += std::string(colSep.size(), ' ');
                    }
                }

            }
            ss<<std::setw(maxColumnLen[col]+columnPadding[col]) << table[row][col].substr(0, table[row][col].find('\n',0));
            exportStr += ss.str();
            if(col<table[row].size()-1) {
                if(col+1<table[row].size() && maxColumnLen[col+1]>0) exportStr += colSep;
                else {
                    exportStr += std::string(colSep.size(), ' ');
                }
            }
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

std::string createTable::isolatedExport(
    std::vector<std::vector<std::string>> extTable,
    std::string startRow,
    bool createBorder,
    std::string rowEnd,
    std::string colSep
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

int createTable::getCellX(
    int column,
    int row,
    std::string rowSep
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
int createTable::getCellY(
    int column,
    int row,
    std::string rowSep
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
const int* createTable::getCellPos(
    int column,
    int row,
    std::string rowSep
) {
    static int pos[2] = {0, 0};
    pos[0] = getCellX(column, row, rowSep);
    pos[1] = getCellY(column, row, rowSep);

    return pos;
}

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

int createTable::copySettings(
    createTable copyFrom
) {

    return -1;
}

int createTable::copyTo_settings(
    createTable& copyTo
) {
    copyTo.use_label_column = use_label_column;
    copyTo.columnSep = columnSep;
    copyTo.rowSep = rowSep;
    copyTo.border_widthChar = border_widthChar;
    copyTo.border_heightChar = border_heightChar; return -1;
}


int createTable::findOcc_char(std::string toCheck, char toFind) {
    std::string::difference_type n = std::count(toCheck.begin(), toCheck.end(), toFind);
    return static_cast<int>(n);
}

