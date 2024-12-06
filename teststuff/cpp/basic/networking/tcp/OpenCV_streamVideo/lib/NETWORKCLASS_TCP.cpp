
#include "NETWORKCLASS_TCP.hpp"


NETWORKCLASS_TCP::NETWORKCLASS_TCP() {
    this->func_init();
}
NETWORKCLASS_TCP::NETWORKCLASS_TCP(std::string _ipAddress, int _port) {
    this->_server_IPADDRESS = _ipAddress;
    this->_server_PORT = _port;
    this->func_init();
}


bool NETWORKCLASS_TCP::set_IPADDRESS(std::string _ipAddress) {
    if(_init) {
        return false;
    }
    _server_IPADDRESS = _ipAddress;
    return true;
}
bool NETWORKCLASS_TCP::set_PORT(int _port) {
    if(_init) {
        return false;
    }
    _server_PORT;
    return true;
}
std::string NETWORKCLASS_TCP::get_IPADDRESS() {
    return _server_IPADDRESS;
}
int NETWORKCLASS_TCP::get_PORT() {
    return _server_PORT;
}


#if _WIN32
SOCKET*     NETWORKCLASS_TCP::get_serverSocket() { return &_serverSocket; }
SOCKET*     NETWORKCLASS_TCP::get_remoteSocket() { return &_remoteSocket; }
#else
int         NETWORKCLASS_TCP::get_serverSocket() { return _serverSocket; }
int         NETWORKCLASS_TCP::get_remoteSocket() { return _remoteSocket; }
#endif

bool NETWORKCLASS_TCP::func_init() {
    if(this->_init) return true;
#if _WIN32
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);

    if(wsaerr!=0) {
        std::cout << "The winsock dll not found" << std::endl;
        return false;
    }
    else {
        std::cout << "The winsock dll found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }
#else

#endif

    this->_init = true;
}
bool NETWORKCLASS_TCP::func_sockCreate() {
#if _WIN32
    this->_serverSocket = INVALID_SOCKET;
    this->_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(_serverSocket==INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }
    else {
        std::cout << "Socket successfully created" << std::endl;
    }
#else
    if((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "socket() failed!" << std::endl;
        return false;
    }
    else {
        std::cout << "Socket successfully created." << std::endl;
    }
#endif

    return true;
}
bool NETWORKCLASS_TCP::func_bind() {

    _server_sockaddr_in.sin_family = AF_INET;
    _server_sockaddr_in.sin_addr.s_addr = (_server_IPADDRESS=="ANY"? INADDR_ANY : inet_addr(_server_IPADDRESS.c_str()));
    _server_sockaddr_in.sin_port = htons(_server_PORT);
#if _WIN32
    if(bind(_serverSocket, reinterpret_cast<SOCKADDR*>(&_server_sockaddr_in), sizeof(_server_sockaddr_in)) == SOCKET_ERROR) {
        std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(_serverSocket);
        WSACleanup();
        return false;
    }
#else
    if(bind(_serverSocket, (struct sockaddr*)&_serverSocket, sizeof(_serverSocket)) < 0) {
        std::cout << "bind() failed!" << std::endl;
        return false;
    }
#endif
    else std::cout << "Socket successfully bound." << std::endl;

    return true;
}
bool NETWORKCLASS_TCP::func_listen(int _numQueued) {

#if _WIN32
    if(listen(_serverSocket, _numQueued) == SOCKET_ERROR) {
        std::cout << "listen(): Error listening on socket: " << WSAGetLastError() << std::endl;
        return false;
    }
#else
    if(listen(_serverSocket, _numQueued) < 0) {
        std::cout << "listen(): Error listening on socket. " << std::endl;
        return false;
    }
#endif
    else {
        std::cout << "Waiting for new connections..." << std::endl;
    }

    return true;
}
bool NETWORKCLASS_TCP::func_accept() {

#if _WIN32
    if((_remoteSocket = accept(_serverSocket, nullptr, nullptr)) == INVALID_SOCKET) {
        std::cout << "accept() failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }
#else
    if((_remoteSocket = accept(_serverSocket, (struct sockaddr*)&_remoteSocket, (socklen_t*)&_sockAddrLen)) < 0) {
        std::cout << "accept() failed" << std::endl;
        return false;
    }
#endif
    else {
        std::cout << "Successfully ran accept()" << std::endl;
    }

    return true;
}
bool NETWORKCLASS_TCP::func_receive() {

    
}
bool NETWORKCLASS_TCP::func_send() {

}
int NETWORKCLASS_TCP::func_send(const void* _dataBuf, size_t _nBytes, int _flags) {
    int bytesSent = 0;
    bytesSent = send(
        _remoteSocket,
#if _WIN32
        (const char*)_dataBuf,
        (int)_nBytes,
#else
        _dataBuf,
        _nBytes,
#endif
        _flags
    );
}
