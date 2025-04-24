
#include <iostream>
#include <imguiwin.hpp>

using namespace GUINC;


int main(int argc, char** argv) {
    imguiwin test(true);

    while(test.running()) {
        test.newFrame();

        try{
            test.draw()->AddCircle(ImVec2(win_dim.x*0.5, win_dim.y*0.5), 10, IM_COL32(200, 200, 10, 200), 10);
            
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            exit(1);
        }
        


        if(!test.running()) break;
        test.endFrame();
    }
    


    return 0;
}