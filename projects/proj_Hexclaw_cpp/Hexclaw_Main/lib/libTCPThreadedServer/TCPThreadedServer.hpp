#pragma once
#ifndef HPP_HEXCLAW_TCP_THREADED_SERVER
#define HPP_HEXCLAW_TCP_THREADED_SERVER


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


#include <thread>
#include <mutex>
#include <atomic>

#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include <cstring> // For memset
#include <stdexcept> // For exceptions


// #include <opencv2/opencv.hpp>

#define TCPThreadedServer__DEFAULT_IPADDR   "ANY"
#define TCPThreadedServer__DEFAULT_PORT     1086
#define TCPThreadedServer__MAX_MESSAGE_SIZE 255

namespace TCPTS {

    template<class _buffType>
    class TCPThreadedServer {
        public:
            TCPThreadedServer(bool start, int port);
            ~TCPThreadedServer();

            bool start();
            bool stop();

            bool isRunning();
            bool isInit();

            void setPort(int port);

            int getPort();
            _buffType getBuffer();

            void updateBuffer(_buffType buf, size_t buf_size);
 
        private:
            
            void threadFunc();

            _buffType stored_buf_;
            size_t stored_buf_size_ = 0;

            int port_;
            
            int port_client_;
            int std::string ipaddr_client_;

            bool isInit_ = false;

            std::atomic<bool> running_{false};
            std::thread thread_;

            std::mutex mtx_dataAccess_buf_;
            std::mutex mtx_dataAccess_clientInfo;


            #if _WIN32
            SOCKET socket_server_;
            SOCKET socket_client_;
            #else
            int socket_server_;
            int socket_client_;
            #endif // _WIN32

            struct sockaddr_in server_addr_;
            struct sockaddr_in client_addr_;

            int sockAddrLen_;

            char sendBuf_[TCPThreadedServer__MAX_MESSAGE_SIZE];
            char recvBuf_[TCPThreadedServer__MAX_MESSAGE_SIZE];

            int bytesReceived_;
            int bytesSent_;
            
            bool socketOpen_server_ = false;
            bool socketOpen_client_ = false;

    };

} // namespace TCPImageServer


/// --------------- Member definition ------------------ ///

namespace TCPTS {

    template<class _buffType>
    TCPThreadedServer<_buffType>::TCPThreadedServer(bool start, int port)
        : port_(port), running_(false), sockAddrLen_(sizeof(server_addr_)) {
        // Initialize sockets and buffers


        if (start) {
            this->start();
        }
        isInit_ = true;
    }

    template<class _buffType>
    TCPThreadedServer<_buffType>::~TCPThreadedServer() {
        stop();
    }

    template<class _buffType>
    bool TCPThreadedServer<_buffType>::start() {
        if (running_) return running_;
        if (isInit_) return false;

#if _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            return false;
        }
#else

#endif

        memset(&server_addr_, 0, sizeof(server_addr_));
        memset(&client_addr_, 0, sizeof(client_addr_));
        memset(sendBuf_, 0, TCPThreadedServer__MAX_MESSAGE_SIZE);
        memset(recvBuf_, 0, TCPThreadedServer__MAX_MESSAGE_SIZE);

#if _WIN32
        socket_server_ = INVALID_SOCKET;
        socket_client_ = INVALID_SOCKET;
#else
        socket_server_ = -1;
        socket_client_ = -1;
#endif // _WIN32
        socket_server_ = socket(AF_INET, SOCK_STREAM, 0);
        
#if _WIN32
        if (socket_server_ == INVALID_SOCKET) {
            WSACleanup();
            throw std::runtime_error("Socket creation failed: " + std::to_string(WSAGetLastError()));
        }
        BOOL _optVal = TRUE;
        if(setsockopt(socket_server_, SOL_SOCKET, SO_REUSEADDR, (char*)&_optVal, sizeof(_optVal))) {
            closesocket(socket_server_);
            WSACleanup();
            throw std::runtime_error("setsockopt failed: " + std::to_string(WSAGetLastError()));
        }
#else
        if (socket_server_ <  0) {
            throw std::runtime_error("Socket creation failed: " + std::to_string(errno));
        }
        int _optVal = 1;
        if (setsockopt(socket_server_, SOL_SOCKET, SO_REUSEADDR, (char*)&_optVal, sizeof(_optVal))) {
            close(socket_server_);
            throw std::runtime_error("setsockopt failed: " + std::to_string(errno));
        }
#endif
        socketOpen_server_ = true;

        server_addr_.sin_family = AF_INET;
        server_addr_.sin_port = htons(port_);
        server_addr_.sin_addr.s_addr = (strcmp(TCPThreadedServer__DEFAULT_IPADDR, "ANY") == 0) ? INADDR_ANY : inet_addr(TCPThreadedServer__DEFAULT_IPADDR);

#if _WIN32
        if (bind(socket_server_, reinterpret_cast<SOCKADDR*>(&server_addr_), sizeof(server_addr_)) == SOCKET_ERROR) {
            closesocket(socket_server_);
            WSACleanup();
            throw std::runtime_error("Bind failed: " + std::to_string(WSAGetLastError()));
        }
#else
        if (bind(socket_server_, (const struct sockaddr*)&server_addr_, sizeof(server_addr_)) < 0) {
            close(socket_server_);
            throw std::runtime_error("Bind failed: " + std::to_string(errno));
        }
#endif // _WIN32

        running_ = true;
        thread_ = std::thread(&TCPThreadedServer::threadFunc, this);
        return running_;
    }

    template<class _buffType>
    bool TCPThreadedServer<_buffType>::stop() {
        if (!running_) return true;

        running_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
        
        #if _WIN32
        if (socket_client_ != INVALID_SOCKET) closesocket(socket_client_);
        if (socket_server_ != INVALID_SOCKET) closesocket(socket_server_);
        WSACleanup();
        #else
        if (socket_client_ != -1) close(socket_client_);
        if (socket_server_ != -1) close(socket_server_);
        #endif

        return true;
    }

    template<class _buffType>
    bool TCPThreadedServer<_buffType>::isRunning() {
        return running_;
    }

    template<class _buffType>
    bool TCPThreadedServer<_buffType>::isInit() {
        return isInit_;
    }

    template<class _buffType>
    void TCPThreadedServer<_buffType>::setPort(int port) {
        port_ = port;
    }


    template<class _buffType>
    int TCPThreadedServer<_buffType>::getPort() {
        return port_;
    }

    template<class _buffType>
    _buffType TCPThreadedServer<_buffType>::getBuffer() {
        // std::lock_guard<std::mutex> lock(mtx_dataAccess_buf_);
        return stored_buf_;
    }

    template<class _buffType>
    void TCPThreadedServer<_buffType>::updateBuffer(_buffType buf, size_t buf_size) {
        std::lock_guard<std::mutex> lock(mtx_dataAccess_buf_);
        stored_buf_ = buf;
        stored_buf_size_ = buf_size;
    }


    template<class _buffType>
    void TCPThreadedServer<_buffType>::threadFunc() {
        std::unique_lock<std::mutex> u_lck_clientInfo(mtx_dataAccess_clientInfo, std::defer_lock);
        std::unique_lock<std::mutex> u_lck_buf(mtx_dataAccess_buf, std::defer_lock);

        while (running_) {
            /// Listen and Accept a client connection
#if _WIN32
            if (listen(socket_server_, 1) == SOCKET_ERROR) {
                closesocket(socket_server_);
                WSACleanup();
                throw std::runtime_error("Listen failed: " + std::to_string(WSAGetLastError()));
            }
            if ((socket_client_ = accept(socket_server_, client_addr_, &sockAddrLen_)) == INVALID_SOCKET) {
                closesocket(socket_server_);
                WSACleanup();
                throw std::runtime_error("Accept failed: " + std::to_string(WSAGetLastError()));
            }
#else
            if (listen(socket_server_, 1) < 0) {
                close(socket_server_);
                throw std::runtime_error("Listen failed: " + std::to_string(errno));
            }
            if ((socket_client_ = accept(socket_server_, (struct sockaddr*)&client_addr_, (socklen_t*)&sockAddrLen_)) < 0) {
                close(socket_server_);
                throw std::runtime_error("Accept failed: " + std::to_string(errno));
            }
#endif // _WIN32

            socketOpen_client_ = true;
            inet_ntop(AF_INET, &client_addr_.sin_addr, sendBuf_, sizeof(sendBuf_));
            u_lck_clientInfo.lock();
            port_client_ = ntohs(client_addr_.sin_port);
            ipaddr_client_ = std::string(sendBuf_);
            u_lck_clientInfo.unlock();

            /// Expected message size for the initial request msg.
            const size_t msgSize_initialRecv = 10;
            while(true) { /// Loop until the connection is closed

                bytesReceived_ = recv(
                    socket_client_,
#if _WIN32
                    (char*)recvBuf_,
                    msgSize_initialRecv,
#else
                    recvBuf_,
                    msgSize_initialRecv,
#endif //_WIN32
                    MSG_WAITALL
                );
                /// Check if recv was successful
                if (bytesReceived_ <= 0) {
                    std::cerr << "recv() failed or connection closed" << std::endl;
                    exit(1);
                }
                recvBuf_[bytesReceived_] = '\0'; // Null-terminate the received string
                /// Send size of the buf size
                bytesSent_ = send(
                    socket_client_,
#if _WIN32
                    (char*)&stored_buf_size_,
                    sizeof(stored_buf_size_),
#else
                    &stored_buf_,
                    sizeof(stored_buf_size_), MSG_NOSIGNAL |
#endif //_WIN32
                    0
                );
                /// Check if send was successful
                if (bytesSent_ <= 0) {
                    std::cerr << "send() failed" << std::endl;
                    exit(1);
                }

                /// Send the buffer data
                u_lck_buf.lock();
                bytesSent_ = send(
                    socket_client_,
#if _WIN32
                    (char*)&stored_buf_,
                    stored_buf_size_*sizeof(stored_buf_[0]),
#else
                    stored_buf_,
                    stored_buf_size_, MSG_NOSIGNAL |
#endif //_WIN32
                    0
                );
                u_lck_buf.unlock();
                /// Check if send was successful
                if (bytesSent_ <= 0) {
                    std::cerr << "send() failed" << std::endl;
                    exit(1);
                }

            }

            /// Empty out the buffer
            while(recv(socket_client_, recvBuf_, sizeof(recvBuf_), 0) > 0);
            /// Close the client socket
#if _WIN32
            closesocket(socket_client_);
#else
            shutdown(socket_client_, SHUT_RDWR);
            if(close(socket_client_)) {
                std::cerr << "close() failed" << std::endl;
                exit(1);
            }
#endif //_WIN32
            socketOpen_client_ = false;
        }
    }

} // namespace TCPTS

#endif // HPP_HEXCLAW_TCP_THREADED_SERVER