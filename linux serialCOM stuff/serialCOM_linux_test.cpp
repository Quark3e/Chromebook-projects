#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;
using namespace chrono;

int main() {
    string input, data, fromUno;

    FILE *file;
    file = fopen("/dev/ttyUSB0", "w");
    
    while(true) {
        cout << "enter input: ";
        cin >> input;
        if(input == "exit") {
            break;
        }
        data = input;
        fprintf(file, "%s\n", data);
        this_thread::sleep_for(milliseconds(1));
        cout << "\n---the arduino received: \"";
        fscanf(file, "%s", &fromUno);
        cout << fromUno << "\"";
        cout << "\n--------------\n";
    }

    // for(int i=0; i<3; i++) {
    //     fprintf(file, "%d", data[i]); //Writing to the file
    //     fprintf(file, "%c", ','); //To separate digits
    //     this_thread::sleep_for(chrono::milliseconds(1));
    // }
    fclose(file);
}

/*
int readBuffer(FILE *stream)
{
    int c;
    c = fgetc(stream);
    ungetc(c, stream);
    return c;
}
*/