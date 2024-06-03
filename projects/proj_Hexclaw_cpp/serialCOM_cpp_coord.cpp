
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <iostream>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

using namespace std;
using namespace chrono;

int main() {
    string usbName;
    cout << "Enter usb name: ";
    cin >> usbName;

    usbName = "/dev/tty" + usbName;
    const char* c = usbName.c_str();
    int serial_port = open(c, O_RDWR);
    

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

    while(true) {
        string toSend, input;
        cout << "Enter servo rotations, in format: x:y:z: ";
        cin >> input;
        if(input == "exit") { break; }
        toSend = input + "\n";
        unsigned char* msg = (unsigned char*)toSend.c_str();

        cout << "sent:" << msg;
        cout << "msg size: " << toSend.length() << " bytes" << endl;
        write(serial_port, msg, toSend.length());
        char read_buf [256];
        memset(&read_buf, '\0', sizeof(read_buf));

        int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));
        if (num_bytes < 0) {
            printf("Error reading: %s", strerror(errno));
            return 1;
        }
        printf("---------------\n");
        printf("Read %i bytes. Received message: %s\n", num_bytes, read_buf);
    }
    close(serial_port);
    return 0; // success
};