#pragma once
#ifndef HPP__NETWORKCLASS
#define HPP__NETWORKCLASS


#if _WIN32
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996) 


#include <winsock2.h>
#include <ws2tcpip.h>
#include <errno.h>
#else
#include <bits/stdc++.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#endif

#define DEFAULT_IPADDR      "ANY"
#define DEFAULT_PORT        1086
#define MAX_MESSAGE_SIZE    512

#include <iostream>
#include <string.h>
#include <signal.h>



// Global variable definitions
class NETWORKCLASS {
    private:
    bool _init = false;

#if _WIN32
    SOCKET  _localSocket;
    SOCKET  _remoteSocket;
#else
    int     _localSocket;
    int     _remoteSocket;
#endif
    std::string _local_IPADDRESS   = "ANY";
    int         _local_PORT        = 1086;

    public:
    sockaddr_in _local_sockaddr_in;
    sockaddr_in _remote_sockaddr_in;

    bool socket_closed = true;

    int _sockAddrLen = sizeof(sockaddr_in);

    char sendBuffer[MAX_MESSAGE_SIZE];
    char recvBuffer[MAX_MESSAGE_SIZE];

    int _bytesSent = 0;
    int _bytesRecv = 0;

    bool        set_IPADDRESS(std::string _ipAddress);
    bool        set_PORT(int _port);
    std::string get_IPADDRESS();
    int         get_PORT();

#if _WIN32
    SOCKET&     get_localSocket();
    SOCKET&     get_remoteSocket();
#else
    int&        get_localSocket();
    int&        get_remoteSocket();
#endif

    /**
     * @brief Construct a new tcpnetwork class object and initialize with default ipAddress/port values
     * 
     */
    // NETWORKCLASS();
    NETWORKCLASS(std::string _ipAddress=DEFAULT_IPADDR, int _port=DEFAULT_PORT);
    ~NETWORKCLASS();

    int     func_init();
    int     func_createSocket(int _sock_family, int _sock_type, int _sock_proto=0);
    int     func_connect();
    int     func_bind();
    int     func_listen(int _numQueued=1);
    int     func_accept();
    int     func_recv(int recvFrom=1);
    int     func_recv(int recvFrom, void* _recvBuf, size_t _nBytes, int _flags);
    int     func_send(int sendTo=1);
    int     func_send(int sendTo, const void* _sendBuf, size_t _nBytes, int _flags);
#if _WIN32
    int     func_recvfrom(SOCKET _sock, void* _sendBuf, size_t _nBytes, int _flags, sockaddr* _from_addr, void* _from_addr_len);
    int     func_sendto(SOCKET _sock, const void* _sendBuf, size_t _nBytes, int _flags, const sockaddr* _to_addr, int _to_addr_len);
#else
    int     func_recvfrom(int    _sock, void* _sendBuf, size_t _nBytes, int _flags, sockaddr* _from_addr, void* _from_addr_len);
    int     func_sendto(int    _sock, const void* _sendBuf, size_t _nBytes, int _flags, const sockaddr* _to_addr, int _to_addr_len);
#endif
    
};


// inline int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr) {
//     int result;
//     addrinfo* result_list = NULL;
//     addrinfo hints = {};
//     hints.ai_family = family;
//     hints.ai_socktype = SOCK_DGRAM;
//     result = getaddrinfo(hostname, service, &hints, &result_list);
//     if(result==0) {
//         //ASSERT(result_list->ai_addrlen<=sieof(sockaddr_in));
//         memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
//         freeaddrinfo(result_list);
//     }
//     return result;
// }


#endif