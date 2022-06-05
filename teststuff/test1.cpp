
#include <iostream>
#include <string>

using namespace std;

int main() {
    string input;
    printf("hello\n");
    
    while(true) {
        cout << "input: ";
        cin >> input;
        if(input == "exit") { break; }
        cout << "---------- " << input << endl;
    }
    return 0;
}