/**
* usb serial port communication to arduino-esc serial device
*/


#include <stdio.h>
#include <stdarg.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <string>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <ctime>

using namespace std;
using namespace cv;
using namespace chrono;

int iLowH = 23;
int iHighH = 80;

int iLowS = 73;
int iHighS = 186;

int iLowV = 18;
int iHighV = 92;

#include "functions.h"

int main() {
    string usbName, input;
    cout << "Enter usb name: ";
    cin >> input;
    if(input == "exit") {
        return 0;
    }
    usbName = input;
    cin.clear();
    cin.ignore();
    usbName = "/dev/tty" + usbName;
    const char* c = usbName.c_str();
    int serial_port = open(c, O_RDWR);

    cout << "\nEnter: iLowH, iHighH, iLowS, iHighS, iLowV, iHighV\ninput: ";
    cin >> iLowH >> iHighH >> iLowS >> iHighS >> iLowV >> iHighV;

    struct termios tty;

    if(tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 0;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }
    sleep(1);

    colorTracking(serial_port);
    /*Mat image;
    namedWindow("Display window");
    VideoCapture cap(0); //0 is the raspicam, 1 is external webcam

    if(!cap.isOpened()) {
        cout << "cannot open camera\n";
    }
    while(true) {
        cap >> image;
        flip(image, image, 1);
        imshow("Display window", image);

        int key = waitKey(20);
        key = (key==255) ? -1 : key;
        if (key>=0)
            break;
    }*/
    return 0;
}
