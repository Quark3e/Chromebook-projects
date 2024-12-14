#pragma once
#ifndef HPP__NETWORK_DATA_THREADCLASS
#define HPP__NETWORK_DATA_THREADCLASS


#if _WIN32
#pragma comment(lib, "Ws2_32.lib")

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
#define MAX_MESSAGE_SIZE    255

#include <iostream>
#include <string.h>
#include <signal.h>
#include <vector>

#include <thread>
#include <mutex>
#include <atomic>

#include <cassert>

#include <jpeglib.h>


extern std::mutex mtx_cout;
inline void mtx_print(std::string _toPrint, bool _blocking=true, bool _flush=true, std::string _end="\n") {
    std::unique_lock<std::mutex> u_lck_cout(mtx_cout, std::defer_lock);

    if(_blocking) {
        u_lck_cout.lock();
        std::cout<<_toPrint<<_end;
        if(_flush) std::cout.flush();
        u_lck_cout.unlock();
    }
    else if(u_lck_cout.try_lock()) {
        std::cout<<_toPrint<<_end;
        if(_flush) std::cout.flush();
        u_lck_cout.unlock();
    }

}


// Global variable definitions
class NETWORK_DATA_THREADCLASS {
    private:
    public:
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

    sockaddr_in _local_sockaddr_in;
    sockaddr_in _remote_sockaddr_in;

    int _sockAddrLen = sizeof(sockaddr_in);

    char sendBuffer[MAX_MESSAGE_SIZE];
    char recvBuffer[MAX_MESSAGE_SIZE];

    int _bytesSent = 0;
    int _bytesRecv = 0;


    bool    func_createSocket(int _sock_family, int _sock_type, int _sock_proto=0);
    bool    func_connect();
    bool    func_bind();
    bool    func_listen(int _numQueued=1);
    bool    func_accept();
    bool    func_recv(int recvFrom=1);
    int     func_recv(int recvFrom, void* _recvBuf, size_t _nBytes, int _flags);
    bool    func_send(int sendTo=1);
    int     func_send(int sendTo, const void* _sendBuf, size_t _nBytes, int _flags);
#if _WIN32
    int     func_recvfrom(SOCKET _sock, void* _sendBuf, size_t _nBytes, int _flags, sockaddr* _from_addr, void* _from_addr_len);
    int     func_sendto(SOCKET _sock, const void* _sendBuf, size_t _nBytes, int _flags, const sockaddr* _to_addr, int _to_addr_len);
#else
    int     func_recvfrom(int    _sock, void* _sendBuf, size_t _nBytes, int _flags, sockaddr* _from_addr, void* _from_addr_len);
    int     func_sendto(int    _sock, const void* _sendBuf, size_t _nBytes, int _flags, const sockaddr* _to_addr, int _to_addr_len);
#endif
    
    bool _closing();

    std::thread threadObj;

    unsigned long           img_size;
    std::vector<uint8_t>    imgArr_sub;

    std::atomic<bool>   running{true};
    /**
     * The boolean for the thread function to listen and check whether to pull data
     * from the image source over network and process it
     */
    std::atomic<bool>   runLoop{false};
    std::atomic<bool>   isRunning{false};

    std::atomic<bool>   imgInit{false};


    /**
     * Indexing value to tell where in the image processing stages the sub-thread loop is at
    */
    std::atomic<int>        _processIdx{0};
    /// @brief vector containing the data
    std::vector<uint8_t>    imgArr;
    /// @brief Mutex for accessing imgArr data and it's `img*` relatives
    std::mutex              imgMutex;

    /**
     * @brief Construct a new tcpnetwork class object and initialize with default ipAddress/port values
     */
    // NETWORK_DATA_THREADCLASS();
    NETWORK_DATA_THREADCLASS(bool _init, std::string _ipAddress=DEFAULT_IPADDR, int _port=DEFAULT_PORT);
    ~NETWORK_DATA_THREADCLASS();

    bool    func_init();

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

};


void _NDT_threadFunc(NETWORK_DATA_THREADCLASS* ndt_obj);

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