
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <vector>

using namespace std;


void splitString(string line, char delimiter, float returnArr[4], int numVar=4) {
    cout << line << "      \n";
    for(int i=0; i<numVar-1; i++) {
        returnArr[i] = stof(line.substr(0, line.find(delimiter)));
        line.erase(line.find(delimiter)+1);
    }
    returnArr[numVar-1] = stof(line);
}

void loadData() {

    int columns=4;

    fstream csvFile;
    csvFile.open("csv_[1,1,1]_6568781_p2_artificial.csv", ios::in);

    int rowCount=0;

    vector<string> row;
    string line;
    float var[4], tempArr[4];
    string strArr[4];

    getline(csvFile, line);

    while(getline(csvFile, line)) {
        // printf("\"%s\n\"test", line.c_str());
        cout << "\"" << line.c_str() << "\"      \n";
        rowCount++;
        // splitString(line, ',', tempArr, columns);
        // printf("- %2d: x:%3d y:%3d z:%4d area:%0.4f\n", rowCount, int(tempArr[0]),int(tempArr[1]),int(tempArr[2]),tempArr[3]);
        // for(int i=0; i<4; i++) cout << strArr[i] << " ";
    }
}

int main() {
    loadData();

    return 0;
}