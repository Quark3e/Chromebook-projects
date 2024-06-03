
#include <iostream>
#include <string>

using namespace std;

int main() {
    system("cls");
    string input;
    printf("hello\n");

    const char* test = "69";
    
    while(true) {
        printf("%c", *test);
        cout << endl;
        cout << "input: ";
        cin >> input;
        if(input == "exit") { break; }
        cout << "---------- " << input << endl;
        system("pause");
    }
    return 0;
}