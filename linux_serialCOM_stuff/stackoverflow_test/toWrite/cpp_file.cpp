#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

int main() {
    
    FILE *file;
    //Opening device file

    string getnum;
    string input;
    
    string usbName;
    cout << "Enter usb name: ";
    cin >> usbName;

    // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
    usbName = "/dev/tty" + usbName;
    const char* c = usbName.c_str();
    file = fopen(c, "w");

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