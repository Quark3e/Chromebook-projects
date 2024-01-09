
#include <iostream>
#include "../useful.hpp"

using namespace std;


int main(int argc, char** argv) {

    vector<float> testArr = {9, 2, 3, 6, 1, 10, 7, 5, 10, 8};
    int findVal = 4;

    int idx = getClosestValIdx(testArr, findVal);
    printf("find value:%d | found idx:[%d]; value of found idx:%f\n", findVal, idx, testArr.at(idx));


    return 0;
}