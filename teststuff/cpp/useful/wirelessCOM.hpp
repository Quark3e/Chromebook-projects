
#pragma once
#ifndef H_WIRELESS_COM
#define H_WIRELESS_COM


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <csignal>
#include <stdio.h>
#include <iostream>
#include <math.h>

#define MAXLINE 2048


class nodemcu_connect {
    private:
    int bind_result;
    int sock;
    const char* PORT = "53";
    const char* ADDRESS = "192.168.1.118";
    char szIP[100];
    sockaddr_in addrListen;
    sockaddr_storage addrDest;
    const char* toESP_msg;

    public:
    char buffer[MAXLINE];

    int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr);
    void nodemcu_udp_setup();
    nodemcu_connect(const char* board_ip="192.168.1.118", const char* board_port="53") {
        ADDRESS = board_ip;
        PORT = board_port;

        nodemcu_udp_setup();
    }
    int receive(bool printResult);
};


class nodemcu_orient {
    public:
    
    // float orient[3] = {0,0,0}; //degrees
    float x_accel, y_accel, z_accel, pitch, roll, Pitch=0, Roll=0;
    bool useFilter = false;
    float accelFilter = 0.1;

    float* orientPtr;

    nodemcu_connect connectObj;
    const char* PORT = "53";
    const char* ADDRESS = "192.168.1.118";

    nodemcu_orient(
        float ptrOrient[3],
        const char* board_address="192.168.1.118",
        const char* board_port="53"
        ): connectObj(board_address, board_port) {
            orientPtr = ptrOrient;
            PORT = board_port;
            ADDRESS = board_address;
    }
    void update(bool printResult);
};


#endif