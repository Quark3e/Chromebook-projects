
#include <iostream>
#include <string>

using namespace std;

class test {
    float* varCopy;

    public:
    test(float* var0) {
        varCopy = var0;
    }
    void updateVar();
};

void test::updateVar() {
    *varCopy+=2;
}


int main(int argc, char** argv) {
    float orient = 0;
    test toTest(&orient);

    for(int i=0; i<10; i++) {
        cout << orient << endl;
        toTest.updateVar();
    }
    cout << orient << endl;

    return 0;
}