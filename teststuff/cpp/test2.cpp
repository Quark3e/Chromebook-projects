#include <iostream>
#include <stdio.h>

using namespace std;


int main() {
    unsigned char *test;
    cout << "input: ";
    cin >> test;
    cout << "\nsize: " << sizeof(test) << " output: " << test;

    cout << endl;
    return 0;
}