
#include <string>
#include <iostream>
#include <vector>

using namespace std;

void testFunc(vector<string>* vecArg) {
    for(int i=0; i<(*vecArg).size(); i++) {
        cout << (*vecArg).at(i) << endl;
        (*vecArg).at(i)+=" [modded]";
    }
}


int main(int argc, char** argv) {
    vector<string> inp;
    for(int i=0; i<20; i++) {
        inp.push_back("idx:"+to_string(i));
    }
    testFunc(&inp);
    cout << "------------\n";
    for(int i=0; i<inp.size(); i++) {
        cout << inp.at(i) << endl;
    }

    return 0;
}