
#include <iostream>
#include <winsock2.h>

#define DEFAULT_IPADDR  "192.168.1.220"
#define DEFAULT_PORT    1086



int main(int argc, char** argv) {
    // Initialize WSA variables
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);

    // Check for initialization success
    if(wsaerr!=0) {
        std::cerr << "The Winsock dll not found." << std::endl;
        exit(1);
    }
    else {
        std::cout << "The Winsock dll found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }

    // Socket creation
    SOCKET serverSocket;
    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Check for socket creation success
    if(serverSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }
    else {
        std::cout << "Socket is successfully created." << std::endl;
    }

    // Bind socket to an IP address and port number
    sockaddr_in service;
    service.sin_family = AF_INET;
    // service.sin_addr.s_addr = inet_addr(DEFAULT_IPADDR);
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(DEFAULT_PORT);

    // Use the bind function
    if(bind(serverSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)) == SOCKET_ERROR) {
        std::cerr << "bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        exit(1);
    }
    else {
        std::cout << "bind() was successful" << std::endl;
    }

    // Listen for incoming connections
    if(listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cout << "listen(): Error listening on socket: " << WSAGetLastError() << std::endl;
    }
    else {
        std::cout << "listen() is Ok. Waiting for new connections.." << std::endl;
    }

    // Accept incoming connections
    SOCKET acceptSocket;
    acceptSocket = accept(serverSocket, nullptr, nullptr);
    
    // Check for successful connection
    if(acceptSocket==INVALID_SOCKET) {
        std::cout << "accept() failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }
    else {
        std::cout << "Successfully ran accept()" << std::endl;
    }

    // Receive data from the client
    char receiveBuffer[200];
    int rByteCount = recv(acceptSocket, receiveBuffer, 200, 0);
    if(rByteCount<0) {
        std::cerr << "Server recv error: " << WSAGetLastError() << std::endl;
        exit(1);
    }
    else {
        std::cout << "Received data: " << receiveBuffer << std::endl;
    }

    // Send a response to the client
    char sendBuffer[200];
    std::cout << "Enter the message: ";
    std::cin.getline(sendBuffer, 200);
    int sbyteCount = send(acceptSocket, sendBuffer, 200, 0);
    if(sbyteCount==SOCKET_ERROR) {
        std::cerr << "Server send() error: " << WSAGetLastError() << std::endl;
        exit(1);
    }
    else {
        std::cout << "Server: sent " << sbyteCount << " bytes" << std::endl;
    }


    return 0;
}