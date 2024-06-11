
#include <iostream>
#include <string>
#include <vector>

#include <HC_useful/useful.hpp>
#include <HC_useful/diy_dictionary.hpp>
// #include "../terminalMenu.hpp"
#include <HC_useful/search_multithread.hpp>

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
        // ANSI_mvprint(10, 20, globStr);
        newStr = false;
    }
}

DIY::dict dictObj_glob(
    std::vector<string>{"uno", "dos", "tres"},
    std::vector<bool>{true, true, false}
);

int main(int argc, char** argv) {

    cout << "program called:-----------"<<endl;


    DIY::dict dictObj;

    std::vector<std::string> testVec{"one", "two", "three", "two", "five", "two", "two"};
    std::string toFind = "";


    dictObj.add("str vecPtr 0", &testVec);
    dictObj.add("toFind 0", &toFind);

    dictObj.get1_stringP("str vecPtr 0")->at(0) = "amogus";
    dictObj.get1_stringP("str vecPtr 0")->at(2) = "pepega";
    dictObj.get1_stringP("str vecPtr 0")->at(4) = "amogus";


    // (*dictObj.get0_stringP("toFind 0")) = "pepega";


    if(dictObj.add("toFind 1", std::string("pepega"))==1) return 1;


    dictObj.delete_key("toFind 0");


    dictObj.edit("toFind 1", std::string("kekw"));

    testVec[2] = "kekw";



    std::vector<int> foundIdx = DIY_SEARCH_MULTITHREAD::multithread_searchVec<std::string>(
        *dictObj.get1_stringP("str vecPtr 0"), dictObj.get0_string_("toFind 1"), -1, 6, true, 1, true
    );
    
    
    std::string outStr = dictObj.str_export("toFind 1");
    if(foundIdx.at(0)==-1) cout << "toFind value \""<<outStr<<"\" not found." << endl;
    else {
        cout << "Indices found. Found \""<<outStr<<"\" at:\n";
        for(size_t i=0; i<foundIdx.size(); i++) {
            cout << " > [" << foundIdx.at(i) << "] -> key of idx:\""<<testVec[foundIdx.at(i)] <<"\" -> "<< dictObj.str_export(dictObj[0]) <<" \n";
        }
    }

    cout << "---------" << endl;

    cout << DIY::prettyPrint_vec1<std::string>(dictObj_glob.keys(),"right",2,0,0,0,4,true) << endl;
    for(std::string key: dictObj_glob.keys()) {
        cout << DIY::formatNumber<std::string>(key,6,0,"left")<<": ";
        cout << std::boolalpha << dictObj_glob.get0_bool_(key) << endl;
    }
    


    return 0;
}