#pragma once
#ifndef HPP__NETWORKCLASS_TCP
#define HPP__NETWORKCLASS_TCP


#if _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#define DEFAULT_IPADDR      "ANY"
#define DEFAULT_PORT        1086
#define MAX_MESSAGE_SIZE    255

#include <iostream>
#include <unistd.h>
#include <string.h>

// Global variable definitions
class NETWORKCLASS_TCP {
    private:
    bool _init = false;

#if _WIN32
    SOCKET  _serverSocket;
    SOCKET  _remoteSocket;
#else
    int     _serverSocket;
    int     _remoteSocket;
#endif
    sockaddr_in _server_sockaddr_in;
    sockaddr_in _remote_sockaddr_in;

    int _sockAddrLen = sizeof(sockaddr_in);

    std::string _server_IPADDRESS   = "ANY";
    int         _server_PORT        = 1086;

    public:
    char sendBuffer[MAX_MESSAGE_SIZE];
    char recvBuffer[MAX_MESSAGE_SIZE];

    bool        set_IPADDRESS(std::string _ipAddress);
    bool        set_PORT(int _port);
    std::string get_IPADDRESS();
    int         get_PORT();

#if _WIN32
    SOCKET*     get_serverSocket();
    SOCKET*     get_remoteSocket();
#else
    int         get_serverSocket();
    int         get_remoteSocket();
#endif

    /**
     * @brief Construct a new tcpnetwork class object and initialize with default ipAddress/port values
     * 
     */
    NETWORKCLASS_TCP();
    NETWORKCLASS_TCP(std::string _ipAddress=DEFAULT_IPADDR, int _port=DEFAULT_PORT);

    bool func_init();
    bool func_sockCreate();
    bool func_bind();
    bool func_listen(int _numQueued=1);
    bool func_accept();
    bool func_receive();
    bool func_send();
    int func_send(const void* _dataBuf, size_t _nBytes, int _flags);

};


#endif