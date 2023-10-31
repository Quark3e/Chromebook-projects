
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <vector>
#include <locale>

using namespace std;


void splitString(string line, string delimiter, float returnArr[4], int numVar=4, bool printVar=true) {
    if(printVar) cout << "--- \"" << line << "\"\n";
    size_t pos = 0;
    for(int i=0; i<numVar; i++) {
        if(i<(numVar-1)) pos = line.find(delimiter);
        if(printVar) cout << "- pos:" << pos << " :" << line.substr(0, pos) << "\n";
        returnArr[i] = stof(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
    }
    if(printVar) cout << "---";
}


float artifVal[401][181][181]; //x = [0, 90, 181] = [-90, 0, 01]

void loadData(bool printVar=true, string filename="csv_[1,1,1]_6568781_p3_artificial.csv") {

    cout << "Starting to load the data\n";

    int columns=4;

    fstream csvFile;
    csvFile.open("csv_[1,1,1]_6568781_p3_artificial.csv", ios::in);

    int rowCount=0;
    char temp[16]="0123456789;,.- ";

    string line;
    float tempArr[4];
    string strArr[4];

    getline(csvFile, line);
    while(getline(csvFile, line)) {
        int idx=0;
        bool fullbreak = false;
        for(int n=0; n<100; n++) {
            for(int i=0; i<sizeof(temp)/sizeof(temp[0]); i++) {
                if(line[n] == temp[i]) break;
                else if (i>=sizeof(temp)/sizeof(temp[0])-1) {
                    idx=n;
                    fullbreak = true;
                    break;
                }
            }
            if(fullbreak) break;
        }

        if(printVar) printf("%7d |%36s|", rowCount, line.substr(0, idx).c_str());
        splitString(line.substr(0, idx), ",", tempArr, columns, false);
        if(printVar) printf(" x:%3d y:%3d z:%4d area:%0.4f\n", int(tempArr[0]),int(tempArr[1]),int(tempArr[2]),tempArr[3]);
        artifVal[int(tempArr[2])][int(tempArr[0])+90][int(tempArr[1])+90] = tempArr[3];
        rowCount++;
    }
    cout << "Finished loading the data: Total rows:" << rowCount << endl;
}

int main() {
    loadData(false);

    return 0;
}
