
#include <iostream>

const int* testFunc() {
    static int pos[2] = {69, 420};

    return pos;
}

int main(int argc, char** argv) {

    const int* returArr;
    returArr = testFunc();


    std::cout << "[0]=" << returArr[0] << " [1]="<<returArr[1] << std::endl;

    // returArr[0] = 21;

    std::cout << "[0]=" << returArr[0] << " [1]="<<returArr[1] << std::endl;

    // returArr[1] = 6969;
    returArr = testFunc();

    std::cout << "[0]=" << returArr[0] << " [1]="<<testFunc()[1] << std::endl;

    return 0;
}