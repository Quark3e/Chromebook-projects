#include <iostream>
#include <string>
#include <chrono>
#include <thread>


using namespace std;
using namespace chrono;

int main() {
    string input, fromUno;

    FILE *file;
    file = fopen("/dev/ttyUSB0", "w");
    
    //string data;
    // while(true) {
    //     cout << "enter input: ";
    //     cin >> input;
    //     if(input == "exit") {
    //         break;
    //     }
    //     data = input + "\n";
    //     fprintf(file, "%s", data);
    //     this_thread::sleep_for(milliseconds(1000));
    //     cout << "\n---the arduino received: \"";
    //     fscanf(file, "%s", &fromUno);
    //     cout << fromUno << "\"";
    //     cout << "\n--------------\n";
    // }

    int data[] = {10,69,13};  //Random data we want to send
    // this_thread::sleep_for(milliseconds(2000));
    cout << "The test is sent in: ";
    for(int i=3; i>0; i++) {
        this_thread::sleep_for(milliseconds(1000));
        cout << i << " ";
    }
    cout << "sending...\n";
        for(int i=0; i<3; i++) {
        fprintf(file, "%d", data[i]); //Writing to the file
        fprintf(file, "%c", ','); //To separate digits
        this_thread::sleep_for(milliseconds(1000));
        cout << "\n---the arduino received: \"";
        fscanf(file, "%s", &fromUno);
        cout << fromUno << "\"";
        cout << "\n--------------\n";
    }
    fclose(file);
}

// int readBuffer(FILE *stream)
// {
//     int c;
//     c = fgetc(stream);
//     ungetc(c, stream);
//     return c;
// }
