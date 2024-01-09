
#include <iostream>
#include "../useful.hpp"
#include <string>
#include <vector>

using namespace std;


int main(int argc, char** argv) {

    // vector<float> testVec = {9, 2, 3, 6, 1, 10, 7, 5, 10, 8};
    // float testArr[] = {9, 2, 3, 6, 1, 10, 7, 5, 10, 8};
    // float findVal = 7.5;

    // // int idx = getClosestValIdx(testVec, findVal);
    // int idx = getClosestValIdx(testArr, sizeof(testArr)/sizeof(testArr[0]), findVal);
    // printf("find value:%f | found idx:[%d]; value of found idx:%f\n", findVal, idx, testArr[idx]);


    string fullText = "this is a test omegalul";
    vector<string> solvedRef;
    vector<string> solvedRes = splitString(fullText, "s", solvedRef);
    for(auto vec: solvedRef) cout << vec << endl;
    // cout << "---------\n";
    // for(auto vec: solvedRes) cout << vec << endl;

    return 0;
}