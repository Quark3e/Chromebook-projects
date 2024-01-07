
#include <iostream>


using namespace std;

class uno {
    public:
    uno(string test) {
        cout << "uno:" << test << endl;
    }
    void second() {
        cout << "|uno second|" << endl;
    }
};

class dos {
    public:
    uno unoObj;

    dos(string test): unoObj(test) {
        cout << "dos:" << test << endl;
    }
    void test() {
        unoObj.second();
        cout << "|dos second|" << endl;
    }
};


int main(int argc, char** argv) {

    dos dosObj("Hello World");
    dosObj.test();

    return 0;
}