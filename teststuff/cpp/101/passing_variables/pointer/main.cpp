
#include <iostream>
#include <string>

using namespace std;

class test {
    float* varCopy;

    int* varArrCopy;
    public:
    test(float* var0, int var1[3]) {
        varCopy = var0;
        varArrCopy = var1;
    }
    void updateVar();
};

void test::updateVar() {
    *varCopy+=2;
    for(int i=0; i<3; i++) {
        varArrCopy[i]+=varArrCopy[i];
    }
}


int main(int argc, char** argv) {
    float orient = 0;
    int arr[3] = {1, 2, 3};
    test toTest(&orient, arr);

    for(int i=0; i<10; i++) {
        cout << orient << "[" << arr[0] << "|" << arr[1] << "|" << arr[2] << "]" << endl;
        toTest.updateVar();
    }
    cout << orient << endl;
    cout << arr[0] << "|" << arr[1] << "|" << arr[2] << endl;

    return 0;
}