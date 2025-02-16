
#include <iostream>
#include <chrono>
#include <thread>
#include <wirelessCOM.hpp>
#include <useful.hpp>

nodemcu_orient orientObj;

void printFunc(float var, std::string& _ret) {
    _ret = formatNumber<float>(var, 5, 2, "right");
}

int main(int argc, char** argv) {

    // orientObj = nodemcu_orient();
    orientObj.accel._callFunc = printFunc;
    orientObj.gyro._callFunc = printFunc;

    std::cout << "starting loop:" << std::endl;
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        try {
            orientObj.update(false);
        }
        catch(const std::exception& e) {
            std::cout << e.what() << '\n';
        }
        
        
        
        std::cout << "accel:" << orientObj.accel << " gyro:" << orientObj.gyro << "   ";
        std::cout << formatNumber<float>(orientObj.Roll, 5, 1) << " " << formatNumber<float>(orientObj.Pitch, 5, 1)<< "\n";
    }

    return 0;
}