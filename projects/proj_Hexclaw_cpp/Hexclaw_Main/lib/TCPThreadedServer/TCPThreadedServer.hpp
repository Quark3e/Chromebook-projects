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

/// --------------- Member declarations ------------------ ///

namespace TCPTS {

    template<class _buffType, class _buffPtrType>
    class TCPThreadedServer {
        public:
            TCPThreadedServer(bool start, int port=TCPThreadedServer__DEFAULT_PORT, std::mutex* print_mtx=nullptr);
            ~TCPThreadedServer();

            bool start();
            bool stop();

            bool isRunning();
            bool isInit();
            bool isClientConnected() const;

            void setPort(int port);
            void setDataBufModifFunction(_buffPtrType (*newFunc)(_buffType&)=nullptr);
            void set_callback_clientFound__func(void (*newFunc)(bool)=nullptr);


            int getPort();
            _buffType getBuffer();

            /**
             * @brief Retrieves the IP address of the connected client.
             * 
             * @return std::string The IP address of the client as a string. If no client is connected then it'll throw `std::runtime_error`
             */
            std::string getClientIPaddr();
            int getClientPort();
            

            void updateBuffer(_buffType buf, size_t buf_size);
 
        private:
            
            _buffPtrType (*dataBuf_ptrModifFunc)(_buffType&) = nullptr;

            /**
             * @brief Callback function for the threadFunc loop. If this function isn't defined as `nullptr` then during client connect or disconnect the thread will
             * call this function with either:
             *  - `true` : if a client is found.
             *  - `false`: if a client has disconnected.
             * 
             */
            void (*callback_clientFound_)(bool) = nullptr;

            void threadFunc();

            _buffPtrType    stored_buf_ptr_;
            _buffType       stored_buf_;
            size_t          stored_buf_size_ = 0;

            int port_;
            
            int port_client_;
            std::string ipaddr_client_;

            std::atomic<bool> isInit_{false};
            std::atomic<bool> bufInit_{false}; 
            std::atomic<bool> running_{false};
            std::thread thread_;

            std::mutex mtx_dataAccess_buf_;
            std::mutex mtx_dataAccess_clientInfo_;
            std::mutex mtx_dataAccess_callback_clientFound__func_;

            std::mutex* mtx_print_ = nullptr;

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
            
            std::atomic<bool> socketOpen_server_{false};
            std::atomic<bool> socketOpen_client_{false};

    };

} // namespace TCPImageServer


/// --------------- Member definitions ------------------ ///

namespace TCPTS {

    template<class _buffType, class _buffPtrType>
    TCPThreadedServer<_buffType, _buffPtrType>::TCPThreadedServer(bool start, int port, std::mutex* print_mtx)
        : port_(port), running_(false), sockAddrLen_(sizeof(server_addr_)), mtx_print_(print_mtx) {
        // Initialize sockets and buffers


        if (start) {
            this->start();
        }
        isInit_ = true;
    }

    template<class _buffType, class _buffPtrType>
    TCPThreadedServer<_buffType, _buffPtrType>::~TCPThreadedServer() {
        try {
            if(running_) stop();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            exit(1);
        }
        
    }

    template<class _buffType, class _buffPtrType>
    bool TCPThreadedServer<_buffType, _buffPtrType>::start() {
        if (running_) return running_;
        if (!isInit_) return false;

#if _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
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
        std::cout<<"sock create" << std::endl;
        if (socket_server_ == INVALID_SOCKET) {
            WSACleanup();
            throw std::runtime_error("Socket creation failed: " + std::to_string(WSAGetLastError()));
        }
        BOOL _optVal = TRUE;
        std::cout<<"sock setsockopt" << std::endl;
        if(setsockopt(socket_server_, SOL_SOCKET, SO_REUSEADDR, (char*)&_optVal, sizeof(_optVal))) {
            closesocket(socket_server_);
            WSACleanup();
            throw std::runtime_error("setsockopt failed: " + std::to_string(WSAGetLastError()));
        }
        std::cout<<"sock ioctlsocket begin" << std::endl;
        u_long mode=1;
        if(ioctlsocket(socket_server_, FIONBIO, &mode)==SOCKET_ERROR) {
            closesocket(socket_server_);
            WSACleanup();
            throw std::runtime_error("ioctlsocket failed: " + std::to_string(WSAGetLastError()));
        }
        std::cout<<"sock ioctlsocket success." << std::endl;
#else
        if (socket_server_ <  0) {
            throw std::runtime_error("Socket creation failed: " + std::to_string(errno));
        }
        int _optVal = 1;
        if (setsockopt(socket_server_, SOL_SOCKET, SO_REUSEADDR, (char*)&_optVal, sizeof(_optVal))) {
            close(socket_server_);
            throw std::runtime_error("setsockopt failed: " + std::to_string(errno));
        }
        if(fcntl(socket_server_, F_SETFL, O_NONBLOCK)==-1) {
            close(socket_server_);
            throw std::runtime_error("fcntl failed: " + std::to_string(errno));
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

    template<class _buffType, class _buffPtrType>
    bool TCPThreadedServer<_buffType, _buffPtrType>::stop() {
        if (!running_) throw std::runtime_error("::stop() called when server wasn't running.");

        std::cout << "TCPThreadedServer::stop(): called. Exiting..." << std::endl;
        running_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
        std::cout << "TCPThreadedServer::stop(): closing sockets." << std::endl;
        #if _WIN32
        if (socket_client_ != INVALID_SOCKET) closesocket(socket_client_);
        if (socket_server_ != INVALID_SOCKET) closesocket(socket_server_);
        WSACleanup();
        #else
        if (socket_client_ != -1) close(socket_client_);
        if (socket_server_ != -1) close(socket_server_);
        #endif

        std::cout << "TCPThreadedServer::stop(): successfully closed server." << std::endl;
        return true;
    }

    template<class _buffType, class _buffPtrType>
    bool TCPThreadedServer<_buffType, _buffPtrType>::isRunning() {
        return running_;
    }

    template<class _buffType, class _buffPtrType>
    bool TCPThreadedServer<_buffType, _buffPtrType>::isInit() {
        return isInit_;
    }

    template<class _buffType, class _buffPtrType>
    bool TCPThreadedServer<_buffType, _buffPtrType>::isClientConnected() const {
        return this->socketOpen_client_.load();
    }

    template<class _buffType, class _buffPtrType>
    void TCPThreadedServer<_buffType, _buffPtrType>::setPort(int port) {
        port_ = port;
    }

    template<class _buffType, class _buffPtrType>
    void TCPThreadedServer<_buffType, _buffPtrType>::setDataBufModifFunction(_buffPtrType (*newFunc)(_buffType&)) {
        std::lock_guard<std::mutex> lock(mtx_dataAccess_buf_);
        this->dataBuf_ptrModifFunc = newFunc;
        if(this->dataBuf_ptrModifFunc) {
            stored_buf_ptr_ = this->dataBuf_ptrModifFunc(this->stored_buf_);
        }
    }

    template<class _buffType, class _buffPtrType>
    void TCPThreadedServer<_buffType, _buffPtrType>::set_callback_clientFound__func(void (*newFunc)(bool)) {
        std::lock_guard<std::mutex> lock(mtx_dataAccess_callback_clientFound__func_);
        this->callback_clientFound_ = newFunc;
    }

    template<class _buffType, class _buffPtrType>
    int TCPThreadedServer<_buffType, _buffPtrType>::getPort() {
        return port_;
    }

    template<class _buffType, class _buffPtrType>
    _buffType TCPThreadedServer<_buffType, _buffPtrType>::getBuffer() {
        // std::lock_guard<std::mutex> lock(mtx_dataAccess_buf_);
        return stored_buf_;
    }

    template<class _buffType, class _buffPtrType>
    std::string TCPThreadedServer<_buffType, _buffPtrType>::getClientIPaddr() {
        std::lock_guard<std::mutex> lock(mtx_dataAccess_clientInfo_);
        if(!socketOpen_client_) throw std::runtime_error("getClientIPaddr: no client connected.");
        return ipaddr_client_;
    }
    template<class _buffType, class _buffPtrType>
    int TCPThreadedServer<_buffType, _buffPtrType>::getClientPort() {
        std::lock_guard<std::mutex> lock(mtx_dataAccess_clientInfo_);
        if(!socketOpen_client_) throw std::runtime_error("getClientPort: no client connected.");

        return port_client_;
    }

    template<class _buffType, class _buffPtrType>
    void TCPThreadedServer<_buffType, _buffPtrType>::updateBuffer(_buffType buf, size_t buf_size) {
        std::lock_guard<std::mutex> lock(mtx_dataAccess_buf_);
        stored_buf_ = buf;
        stored_buf_size_ = buf_size;
        if(this->dataBuf_ptrModifFunc) {
            stored_buf_ptr_ = this->dataBuf_ptrModifFunc(this->stored_buf_);
        }
        else {
            stored_buf_ptr_ = &this->stored_buf_;
        }
        bufInit_ = true;
    }


    template<class _buffType, class _buffPtrType>
    void TCPThreadedServer<_buffType, _buffPtrType>::threadFunc() {
        std::unique_lock<std::mutex> u_lck_buf(mtx_dataAccess_buf_, std::defer_lock);
        std::unique_lock<std::mutex> u_lck_clientInfo(mtx_dataAccess_clientInfo_, std::defer_lock);
        std::unique_lock<std::mutex> u_lck_callback_clientFound_(mtx_dataAccess_callback_clientFound__func_, std::defer_lock);
        
        std::unique_lock<std::mutex> u_lck_cout(*mtx_print_, std::defer_lock);
        // if(mtx_print_) u_lck_cout.mutex() = &mtx_print_;

        bool loopInit = true;

        // u_lck_cout.lock();
        // std::cout<<"TCPTS:"<<this<<": threadFunc starting."<<std::endl;
        // u_lck_cout.unlock();

        if(mtx_print_ && loopInit) {
            // u_lck_cout.lock();
            // std::cout<<"TCPTS:"<<this<<": before list."<<std::endl;
            // u_lck_cout.unlock();
        }
        if (listen(socket_server_, 1) == SOCKET_ERROR) {
            closesocket(socket_server_);
            WSACleanup();
            throw std::runtime_error("Listen failed: " + std::to_string(WSAGetLastError()));
        }
        
        while (running_) {
            /// Listen and Accept a client connection
#if _WIN32
            if(mtx_print_ && loopInit) {
                // u_lck_cout.lock();
                // std::cout<<"TCPTS:"<<this<<": before accept loop."<<std::endl;
                // u_lck_cout.unlock();
            }
            SOCKET _acceptCode = INVALID_SOCKET;
            while((_acceptCode=accept(socket_server_, reinterpret_cast<sockaddr*>(&client_addr_), &sockAddrLen_))==INVALID_SOCKET) {
                if(WSAGetLastError()!=WSAEWOULDBLOCK) {
                    throw std::runtime_error("non-blocking accept() gave error != WSAEWOULDBLOCK.");
                }
                if(!running_) {
                    if(mtx_print_ && loopInit) {
                        // u_lck_cout.lock();
                        // std::cout<<"TCPTS:"<<this<<": running_ == false"<<std::endl;
                        // u_lck_cout.unlock();
                    }
                    
                    return;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            
            // if ((socket_client_ = accept(socket_server_, reinterpret_cast<sockaddr*>(&client_addr_), &sockAddrLen_)) == INVALID_SOCKET) {
            //     if(WSAGetLastError()==WSAEWOULDBLOCK) continue;
            //     closesocket(socket_server_);
            //     WSACleanup();
            //     throw std::runtime_error("Accept failed: " + std::to_string(WSAGetLastError()));
            // }
            if(mtx_print_ && loopInit) {
                // u_lck_cout.lock();
                // std::cout<<"TCPTS:"<<this<<": after accept loop."<<std::endl;
                // u_lck_cout.unlock();
            }
#else
            if (listen(socket_server_, 1) < 0) {
                close(socket_server_);
                throw std::runtime_error("Listen failed: " + std::to_string(errno));
            }
            bool exit_acceptLoop = false;
            int _acceptCode = -1;
            for(; _code==-1; _code=accept(socket_server_, (struct sockaddr*)&client_addr_, (socklen_t*)&sockAddrLen_)) {
                if(errno!=EAGAIN || errno!=EWOULDBLOCK) {
                    close(socket_server_);
                    throw std::runtime_error("non-blocking accept() gave error != EAGAIN / EWOULDBLOCK.");
                }
                if(!running_) {
                    exit_acceptLoop = true;
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            if(exit_acceptLoop) break;
            socket_client_ = _acceptCode;
            // if ((socket_client_ = accept(socket_server_, (struct sockaddr*)&client_addr_, (socklen_t*)&sockAddrLen_)) < 0) {
            //     close(socket_server_);
            //     throw std::runtime_error("Accept failed: " + std::to_string(errno));
            // }
#endif // _WIN32

            socketOpen_client_ = true;
            u_lck_callback_clientFound_.lock();
            if(callback_clientFound_) callback_clientFound_(true);
            u_lck_callback_clientFound_.unlock();

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
            u_lck_callback_clientFound_.lock();
            if(callback_clientFound_) callback_clientFound_(false);
            u_lck_callback_clientFound_.unlock();
            
            loopInit = false;
        }
        if(mtx_print_ && loopInit) {
            // u_lck_cout.lock();
            // std::cout<<"TCPTS:"<<this<<": exiting threadFunc"<<std::endl;
            // u_lck_cout.unlock();
        }
        socketOpen_client_ = false;
        u_lck_callback_clientFound_.lock();
        if(callback_clientFound_) callback_clientFound_(false);
        u_lck_callback_clientFound_.unlock();
    }

} // namespace TCPTS

#endif // HPP_HEXCLAW_TCP_THREADED_SERVER