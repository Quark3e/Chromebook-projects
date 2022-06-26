#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

int main() {
    
    FILE *file;
    //Opening device file

    int getnum;
    string input;
    
    file = fopen("/dev/ttyUSB0", "w");

    while (true) {
        cout << ">> ";
        cin >> input;
        if(input == "exit") {
            break;
        }
        cout << endl;
        getnum = stoi(input);
        cin.clear();
        cin.ignore();
        // fprintf(file, "%d\n", getnum); //Writing to the file
        fprintf(file, "%d\n", getnum);
        
    }

    fclose(file);
}