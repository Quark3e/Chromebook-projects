
// //WORKS

// #include <sys/types.h>
// #include <unistd.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <netdb.h>
// #include <memory.h>
// #include <ifaddrs.h>
// #include <net/if.h>
// #include <errno.h>
// #include <stdlib.h>
// #include <iostream>
// #include <fcntl.h>
// #include <poll.h>

#include <NETWORKCLASS.hpp>

#include <chrono>
#include <thread>
#include <useful.hpp>

#define MAXLINE 2048

NETWORKCLASS udpObj;
// int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr)
// {
//     int result;
//     addrinfo* result_list = NULL;
//     addrinfo hints = {};
//     hints.ai_family = family;
//     hints.ai_socktype = SOCK_DGRAM; // without this flag, getaddrinfo will return 3x the number of addresses (one for each socket type).
//     result = getaddrinfo(hostname, service, &hints, &result_list);
//     if (result == 0)
//     {
//         //ASSERT(result_list->ai_addrlen <= sizeof(sockaddr_in));
//         memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
//         freeaddrinfo(result_list);
//     }
//     return result;
// }

bool __VERBOSE = false;

int main(int argc, char** argv) {

    std::string serverIP    = "192.168.1.117"; //"192.168.1.231" - oled esp8266 board
    int         serverPORT  = 1089; //might be 1087 for oled esp8266 board (note sure)

    if(argc>1) {
        if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            std::cout <<"Usage: cv_video_cli -p/--port [port] -a/--address [address]\n" <<
                        " port      : socket port ("<<serverPORT<<" default)\n" <<
                        " address   : server IPaddress (\""<<serverIP<<"\" default (same device))\n" << std::endl;
            exit(0);
        }
        for(int i=1; i<argc; i++) {
            if(i<argc) {
                if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")) {
                    serverPORT = std::stoi(argv[i+1]);
                }
                if(!strcmp(argv[i], "-a") || !strcmp(argv[i], "--address")) {
                    serverIP = argv[i+1];
                }
            }
            if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
                __VERBOSE = true;
            }
        }
    }


    if(__VERBOSE) std::cout << "\tInit" << std::endl;
    
    udpObj = NETWORKCLASS("192.168.1.117", 1089);
    if(__VERBOSE) std::cout << "\tobject init" << std::endl;

    if(udpObj.func_createSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)!= 0) {
        std::cerr << "Could not create socket." << std::endl;
        exit(1);
    }
    udpObj._remote_sockaddr_in.sin_family   = AF_INET;
    udpObj._remote_sockaddr_in.sin_port     = htons(serverPORT);
    udpObj._remote_sockaddr_in.sin_addr.s_addr  = inet_addr(serverIP.c_str());
    
    if(__VERBOSE) std::cout << "\tsocket created" << std::endl;

    int nTimeout = 5000; //ms
// #if _WIN32
    setsockopt(udpObj.get_localSocket(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&nTimeout, sizeof(int));
// #else
//     setsockopt(udpObj.get_localSocket(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&nTimeout, sizeof(int));
// #endif

    const char* sendMsg = "Hello";

    if(__VERBOSE) std::cout << "\tloop start" << std::endl;
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
        int result = udpObj.func_sendto(
            udpObj.get_localSocket(),
            sendMsg,
            strlen(sendMsg),
            0,
            (sockaddr*)&udpObj._remote_sockaddr_in,
            sizeof(udpObj._remote_sockaddr_in)
        );
        if(result==-1) {
            perror("sendto() error: ");
            exit(1);
        }
        std::cout << "sent:"<< result << " bytes. ";

        udpObj.func_recvfrom(
            udpObj.get_localSocket(),
            udpObj.recvBuffer,
            MAX_MESSAGE_SIZE,
            0,
            (sockaddr*)&udpObj._remote_sockaddr_in,
            &udpObj._sockAddrLen
        );
        if(udpObj._bytesRecv < 0) {
#if _WIN32
            std::cout << "recvfrom() error: "<< WSAGetLastError();
#else
            perror("recvfrom() error: ");
#endif
            std::cout << std::endl;
            exit(1);
        }
        std::cout << " received "<<udpObj._bytesRecv << " bytes ";
        udpObj.recvBuffer[udpObj._bytesRecv] = '\0';

        std::cout << "\"" << udpObj.recvBuffer << "\"" << std::endl;
    }

    return 0;
}
