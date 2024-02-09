
#include <iostream>
#include "../../../../../projects/proj_Hexclaw_cpp/in rpi/HW_headers/IR_camTrack.hpp"
#include "../../../../../teststuff/cpp/two_cam_coordinate/two_cam_coordinate.hpp"


int main(int argc, char** argv) {
    int prefSize[2] = {640, 480};
    bool displayToWindow = true;
    bool useAutoBrightne = true;
    bool takePerformance = true;

    float camPosition[2][2] = {{0, 0}, {250, 0}};
    float camAng_offs[2] = {90, 123};
    float inpPos[2], solvedPos[2];

    IR_camTracking camObj[2] {
        (2, prefSie[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance),
        (0, prefSie[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance)
    };

    camTriangle camTri(camPosition, camAng_offs);
    

}