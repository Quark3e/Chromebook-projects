#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

int main()
{
    FILE *file;
    //Opening device file

    int getnum;
    string input;

    file = fopen("/dev/ttyUSB0", "w");

    while (true) {
        cout << ">>" << endl;
        cin >> input;
        if(input == "exit") {
            break;
        }
        getnum = stoi(input);
        cin.clear();
        cin.ignore();
        fprintf(file, "%d\n", getnum); //Writing to the file
    }

    fclose(file);
}