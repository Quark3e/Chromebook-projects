
#include "NETWORKCLASS.hpp"


// NETWORKCLASS::NETWORKCLASS() {
//     this->func_init();
// }
NETWORKCLASS::NETWORKCLASS(std::string _ipAddress, int _port) {
    this->_local_IPADDRESS = _ipAddress;
    this->_local_PORT = _port;
    this->func_init();
}
NETWORKCLASS::~NETWORKCLASS() {
#if _WIN32
    closesocket(_localSocket);
    WSACleanup();
#else
    close(_remoteSocket);
#endif
}


bool NETWORKCLASS::set_IPADDRESS(std::string _ipAddress) {
    if(_init) {
        return false;
    }
    _local_IPADDRESS = _ipAddress;
    return true;
}
bool NETWORKCLASS::set_PORT(int _port) {
    if(_init) {
        return false;
    }
    _local_PORT;
    return true;
}
std::string NETWORKCLASS::get_IPADDRESS() {
    return _local_IPADDRESS;
}
int NETWORKCLASS::get_PORT() {
    return _local_PORT;
}


#if _WIN32
SOCKET&     NETWORKCLASS::get_localSocket() { return _localSocket; }
SOCKET&     NETWORKCLASS::get_remoteSocket() { return _remoteSocket; }
#else
int&        NETWORKCLASS::get_localSocket() { return _localSocket; }
int&        NETWORKCLASS::get_remoteSocket() { return _remoteSocket; }
#endif

bool NETWORKCLASS::func_init() {
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

	memset(&_local_sockaddr_in, 0, sizeof(_local_sockaddr_in));
	memset(&_remote_sockaddr_in, 0, sizeof(_remote_sockaddr_in));

    this->_init = true;
    return true;
}
bool NETWORKCLASS::func_createSocket(int _sock_family, int _sock_type, int _sock_proto) {
#if _WIN32
    this->_localSocket = INVALID_SOCKET;
    this->_localSocket = socket(_sock_family, _sock_type, _sock_proto);

    if(_localSocket==INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }
    else {
        // std::cout << "Socket successfully created" << std::endl;
    }
#else
    if((_localSocket = socket(_sock_family, _sock_type, _sock_proto)) < 0) {
        std::cout << "socket() failed!" << std::endl;
        return false;
    }
    else {
        // std::cout << "Socket successfully created." << std::endl;
    }
#endif
    
    return true;
}
bool NETWORKCLASS::func_connect() {
    std::cout << _local_IPADDRESS << std::endl;

    _remote_sockaddr_in.sin_family = AF_INET;
    _remote_sockaddr_in.sin_addr.s_addr = inet_addr(_local_IPADDRESS.c_str());
    _remote_sockaddr_in.sin_port = htons(_local_PORT);
#if _WIN32
    if(connect(_localSocket, reinterpret_cast<SOCKADDR*>(&_remote_sockaddr_in), sizeof(sockaddr_in)) < 0) {
        std::cout << "connect() failed." << std::endl;
        WSACleanup();
        return false;
    }
#else
    if(connect(_localSocket, (sockaddr*)&_remote_sockaddr_in, sizeof(_remote_sockaddr_in)) < 0) {
        std::cout << "connect() failed." << std::endl;
        return false;
    }
#endif

    return true;
}
bool NETWORKCLASS::func_bind() {

    _local_sockaddr_in.sin_family = AF_INET;
    _local_sockaddr_in.sin_addr.s_addr = (_local_IPADDRESS=="ANY"? INADDR_ANY : inet_addr(_local_IPADDRESS.c_str()));
    _local_sockaddr_in.sin_port = htons(_local_PORT);
#if _WIN32
    if(bind(_localSocket, reinterpret_cast<SOCKADDR*>(&_local_sockaddr_in), sizeof(_local_sockaddr_in)) == SOCKET_ERROR) {
        std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(_localSocket);
        WSACleanup();
        return false;
    }
#else
    if(bind(_localSocket, (const struct sockaddr*)&_local_sockaddr_in, sizeof(_local_sockaddr_in))==-1) {
        std::cout << "bind() failed!" << std::endl;
        return false;
    }
#endif
    // else std::cout << "Socket successfully bound." << std::endl;

    return true;
}
bool NETWORKCLASS::func_listen(int _numQueued) {

#if _WIN32
    if(listen(_localSocket, _numQueued) == SOCKET_ERROR) {
        std::cout << "listen(): Error listening on socket: " << WSAGetLastError() << std::endl;
        return false;
    }
#else
    if(listen(_localSocket, _numQueued) < 0) {
        std::cout << "listen(): Error listening on socket. " << std::endl;
        return false;
    }
#endif
    else {
        std::cout << "Waiting for new connections at: " << std::endl;
        std::cout << " ip   : " << _local_IPADDRESS << std::endl;
        std::cout << " port : " << _local_PORT << std::endl;
    }

    return true;
}
bool NETWORKCLASS::func_accept() {

#if _WIN32
    if((_remoteSocket = accept(_localSocket, nullptr, nullptr)) == INVALID_SOCKET) {
        std::cout << "accept() failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }
#else
    if((_remoteSocket = accept(_localSocket, (struct sockaddr*)&_remoteSocket, (socklen_t*)&_sockAddrLen)) < 0) {
        std::cout << "accept() failed" << std::endl;
        return false;
    }
#endif
    else {
        std::cout << "Successfully accepted connection." << std::endl;
        // getsockname(_remoteSocket, )
    }

    return true;
}
bool NETWORKCLASS::func_recv(int recvFrom) {
    this->func_recv(recvFrom, recvBuffer, sizeof(recvBuffer), 0);
    if(_bytesRecv<0) return false;
    return true;
}
int NETWORKCLASS::func_recv(int recvFrom, void* _recvBuf, size_t _nBytes, int _flags) {
    _bytesRecv = recv(
        (recvFrom==0? _localSocket : _remoteSocket),
#if _WIN32
        (char*)_recvBuf,
        (int)_nBytes,
#else
        _recvBuf,
        _nBytes,
#endif
        _flags
    );

    return _bytesRecv;
}
bool NETWORKCLASS::func_send(int sendTo) {
    this->func_send(sendTo, sendBuffer, sizeof(sendBuffer), 0);
    if(_bytesSent<0) return false;
    return true;
}
int NETWORKCLASS::func_send(int sendTo, const void* _sendBuf, size_t _nBytes, int _flags) {
    _bytesSent = send(
        (sendTo==0? _localSocket : _remoteSocket),
#if _WIN32
        reinterpret_cast<const char*>(_sendBuf),
        (int)_nBytes,
#else
        _sendBuf,
        _nBytes, MSG_NOSIGNAL | 
#endif
        _flags
    );
    return _bytesSent;
}
#if _WIN32
int NETWORKCLASS::func_recvfrom(SOCKET _sock, void* _sendBuf, size_t _nBytes, int _flags, sockaddr* _from_addr, void* _from_addr_len) {
    _bytesRecv = recvfrom(
        _sock,
        reinterpret_cast<char*>(_sendBuf),
        _nBytes,
        _flags,
        _from_addr,
        (int*)_from_addr_len
    );
    return _bytesRecv;
}
int NETWORKCLASS::func_sendto(SOCKET _sock, const void* _sendBuf, size_t _nBytes, int _flags, const sockaddr* _to_addr, int _to_addr_len) {
    _bytesSent = sendto(
        _sock,
        reinterpret_cast<const char*>(_sendBuf),
        int(_nBytes),
        _flags,
        _to_addr,
        _to_addr_len
    );
    return _bytesSent;
}
#else
int NETWORKCLASS::func_recvfrom(int _sock, void* _sendBuf, size_t _nBytes, int _flags, sockaddr* _from_addr, void* _from_addr_len) {
    _bytesRecv = recvfrom(
        _sock,
        (char*)_sendBuf,
        _nBytes,
        _flags,
        _from_addr,
        (socklen_t*)_from_addr_len
    );
    return _bytesRecv;
}
int NETWORKCLASS::func_sendto(int    _sock, const void* _sendBuf, size_t _nBytes, int _flags, const sockaddr* _to_addr, int _to_addr_len) {
    _bytesSent = sendto(
        _sock,
        _sendBuf,
        _nBytes,
        _flags,
        _to_addr,
        _to_addr_len
    );
    return _bytesSent;
}
#endif