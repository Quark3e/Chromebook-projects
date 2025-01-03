/*
god bleaseth!
https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
*/
// C library headers
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
    // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
    int serial_port = open("/dev/ttyUSB0", O_RDWR);
    
    // Create new termios struct, we call it 'tty' for convention
    struct termios tty;

    // Read in existing settings, and handle any error
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
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 1;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }
    sleep(1);

    while(true) {

        // this_thread::sleep_for(milliseconds(1000));
        //format: x:y:z \n 
        //note:

        // Write to serial port
        // unsigned char msg[] = { 'H', 'e', 'l', 'l', 'o', ';', '\r' };
        // string toSend = "x:y:z\n";
        string toSend, input;
        cout << " Enter servo rotations, in format: x:y:z\n";
        cout << " input: ";
        cin >> input;
        if(input == "exit") { break; }
        toSend = input + "\n";

        unsigned char* msg = (unsigned char*)toSend.c_str();
        // unsigned char msg[] = { 0'6', '9', ';', '\r' };

        cout << msg << endl;
        cout << sizeof(msg);
        write(serial_port, msg, sizeof(msg));

        // this_thread::sleep_for(milliseconds(100));

        // char read_buf [256];
        // memset(&read_buf, '\0', sizeof(read_buf));

        // // Read bytes. The behaviour of read() (e.g. does it block?,
        // // how long does it block for?) depends on the configuration
        // // settings above, specifically VMIN and VTIME
        // int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));
        // if (num_bytes < 0) {
        //     printf("Error reading: %s", strerror(errno));
        //     return 1;
        // }
        // printf("Read %i bytes. Received message: %s\n", num_bytes, read_buf);

    }

    close(serial_port);
    return 0; // success
};