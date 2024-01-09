
#include <iostream>
#include "../useful.hpp"

using namespace std;


int main(int argc, char** argv) {

    int testArr[] = {
        1, 2, 3, 10, 5, 10
    };
    int findVal = 6;

    int idx = getClosestValIdx(testArr ,findVal);
    printf("find value:%d | found: idx:%d; value of found idx:%d\n", findVal, idx, testArr[idx]);


    return 0;
}