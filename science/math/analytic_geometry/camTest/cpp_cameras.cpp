
#include <iostream>
#include <cstring>
#include "../../../../../projects/proj_Hexclaw_cpp/in rpi/HW_headers/IR_camTrack.hpp"
#include "../../../../../teststuff/cpp/two_cam_coordinate/two_cam_coordinate.hpp"


int main(int argc, char** argv) {
    bool useCamera = true, useTwoCamClass = true;
    if(argc>1) {
        if(strcmp(argv[1], "0")==0) {
            useCamera = false;
            useTwoCamClass = false;
        }
        else if(strcmp(argv[1], "1")==0) {
            useCamera = true;
            useTwoCamClass = false;
        }
        else if(strcmp(argv[1], "2")==0) {
            useCamera = false;
            useTwoCamClass = true;
        }
    }
    if(useCamera) {
        int prefSize[2] = {640, 480};
        bool displayToWindow = true;
        bool useAutoBrightne = true;
        bool takePerformance = true;

        IR_camTracking camObj[2] {
            (2, prefSie[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance),
            (0, prefSie[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance)
        };
    }

    if(useTwoCamClass) {
        float camPosition[2][2] = {{0, 0}, {250, 0}};
        float camAng_offs[2] = {90, 123};
        float inpPos[2], solvedPos[2];

        camTriangle camTri(camPosition, camAng_offs);
    }

    

}