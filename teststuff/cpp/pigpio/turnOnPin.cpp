
#include <iostream>
#include <pigpio.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]) {
    if(gpioInitialise() <0) return 1;

    gpioSetMode(23, PI_OUTPUT);
    gpioWrite(23, 1);
    cout << "check 1 reached\n";
    usleep(50000);
    cout << "check reached\n";
    gpioWrite(23, 0);
    usleep(50000);
    gpioWrite(23, 1);
    usleep(50000);
    gpioWrite(23, 0);
    usleep(50000);
    gpioWrite(23, 1);
    usleep(50000);
    gpioWrite(23, 0);
    usleep(50000);

    gpioTerminate();
    return 0;
}
