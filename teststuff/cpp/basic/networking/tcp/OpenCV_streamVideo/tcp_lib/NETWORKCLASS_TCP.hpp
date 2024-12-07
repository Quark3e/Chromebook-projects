#pragma once
#ifndef HPP__NETWORKCLASS_TCP
#define HPP__NETWORKCLASS_TCP


#if _WIN32
#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <errno.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#endif

#define DEFAULT_IPADDR      "ANY"
#define DEFAULT_PORT        1086
#define MAX_MESSAGE_SIZE    255

#include <iostream>
#include <string.h>
#include <signal.h>

// Global variable definitions
class NETWORKCLASS_TCP {
    private:
    bool _init = false;

#if _WIN32
    SOCKET  _localSocket;
    SOCKET  _remoteSocket;
#else
    int     _localSocket;
    int     _remoteSocket;
#endif
    sockaddr_in _local_sockaddr_in;
    sockaddr_in _remote_sockaddr_in;

    int _sockAddrLen = sizeof(sockaddr_in);

    std::string _local_IPADDRESS   = "ANY";
    int         _local_PORT        = 1086;

    public:
    char sendBuffer[MAX_MESSAGE_SIZE];
    char recvBuffer[MAX_MESSAGE_SIZE];

    int _bytesSent = 0;
    int _bytesRecv = 0;

    bool        set_IPADDRESS(std::string _ipAddress);
    bool        set_PORT(int _port);
    std::string get_IPADDRESS();
    int         get_PORT();

#if _WIN32
    SOCKET*     get_localSocket();
    SOCKET*     get_remoteSocket();
#else
    int         get_localSocket();
    int         get_remoteSocket();
#endif

    /**
     * @brief Construct a new tcpnetwork class object and initialize with default ipAddress/port values
     * 
     */
    // NETWORKCLASS_TCP();
    NETWORKCLASS_TCP(std::string _ipAddress=DEFAULT_IPADDR, int _port=DEFAULT_PORT);
    ~NETWORKCLASS_TCP();

    bool func_init();
    bool func_sockCreate();
    bool func_connect();
    bool func_bind();
    bool func_listen(int _numQueued=1);
    bool func_accept();
    bool func_receive();
    int func_receive(void* _recvBuf, size_t _nBytes, int _flags);
    bool func_send();
    int func_send(const void* _sendBuf, size_t _nBytes, int _flags);

};


#endif