
#include <iostream>
#include <imguiwin.hpp>

using namespace guiwin_nc;



int main(int argc, char** argv) {
    imguiwin test(true);

    while(test.running()) {
        test.newFrame();



        if(!test.running()) break;
        test.endFrame();
    }
    


    return 0;
}