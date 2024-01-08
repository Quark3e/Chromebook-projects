
#include <iostream>


using namespace std;

class uno {
    protected:
    string varProt;
    public:
    uno(string test) {
        cout << "uno:" << test << endl;
        varProt = "protected:" + test;
    }
    uno() {
        static string test="meme";
        cout << "uno:" << test << endl;
        varProt = "protected:" + test;
    }
    void second() {
        cout << "|uno second|" << endl;
    }
};

class dos {
    public:
    uno unoObj;

    dos(string test): unoObj(test) {
        // unoObj = uno(test);
        cout << "dos:" << test << endl;
    }
    void test() {
        unoObj.second();
        cout << "|dos second|" << endl;
        // cout << unoObj.varProt << ": is readable from here\n"; //can't do this
    }
};


int main(int argc, char** argv) {

    dos dosObj("Hello World");
    dosObj.test();

    return 0;
}