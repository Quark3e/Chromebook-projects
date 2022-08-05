#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

int main() {
    
    FILE *file;
    //Opening device file

    string getnum;
    string input;
    
    file = fopen("/dev/ttyUSB0", "w");

    while (true) {
        cout << "send coods in x:y:z >> ";
        cin >> input;
        if(input == "exit") {
            break;
        }
        getnum = input;
        cin.clear();
        cin.ignore();
        // fprintf(file, "%d\n", getnum); //Writing to the file
        fprintf(file, "%d\n", getnum);
        
    }

    fclose(file);
    return 0;
}