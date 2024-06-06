
#include <iostream>
#include <string>
#include <vector>

#include <HC_useful/useful.hpp>
// #include <HC_useful/diy_dictionary.hpp>
// #include "../terminalMenu.hpp"
#include "../search_multithread.hpp"

using namespace std;


string a_code = "\x1B[";

// termMenu terminalMenu(false, 10, 10);


void func0() {
    cout << a_code+"10;10Hfunc: 0" << std::endl;
    // cin.get();
}
void func1() {
    cout << a_code+"10;10Hfunc: 1" << std::endl;
    // cin.get();
}
void func2() {
    cout << a_code+"10;10Hfunc: 2" << std::endl;
    // cin.get();
}
void func3() {
    cout << a_code+"10;10Hfunc: 3" << std::endl;
    // cin.get();
}

std::string globStr="";
bool newStr = false;

// void funcInp() {
//     globStr = terminalMenu.termInput(1, 120, 20, 20, 1, "left");
//     newStr = true;
// }

void printInp() {
    if(newStr) {
        ANSI_mvprint(10, 20, globStr);
        newStr = false;
    }
}

int main(int argc, char** argv) {

    // std::cout<<"hello world"<<std::endl;

    // int termSize[2] = {0, 0};
    // getTermSize(termSize[0], termSize[1]);


    // terminalMenu.addOpt("0:0", 0, 0, -1, func1);
    // terminalMenu.addOpt("0:1", 0, 1, -1, func1);
    // terminalMenu.addOpt("0:2", 0, 2, -1, func1);

    // terminalMenu.addOpt("input", 0, 3, 'i', funcInp);


    // terminalMenu.setDisplayDim(3, 3, 5, 5, -1);



    // const int* returPos;
    // bool loopExit = false;
    // if(terminalMenu.addOpt("exit", 0, 5, 27, bool_true, &terminalMenu.exitDriver)==1) {
    //     cerr<<"ERROR STUFF";
    //     return 1;
    // }

    // while(true) {
    //     returPos = terminalMenu.driver(15, 5, 0, false, printInp,false);
    //     if(returPos[0]==0 && returPos[1]==5) {
    //         endwin();
    //         break;
    //     }
    //     else if(returPos[1]==2) printInp();
    // }


    std::vector<std::string> testVec{"one", "two", "three", "two", "five", "two", "two"};
    std::string toFind = "two";

    std::vector<int> foundIdx = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(
        testVec, toFind, 2, -1, false, 2
    );
    
    std::cout << std::endl;
    if(foundIdx.at(0)==-1) cout << "toFind not found." << endl;
    else {
        cout << "Indices found. Found at:\n";
        for(size_t i=0; i<foundIdx.size(); i++) {
            cout << " > [" << foundIdx.at(i) << "]\n";
        }
    }


    return 0;
}