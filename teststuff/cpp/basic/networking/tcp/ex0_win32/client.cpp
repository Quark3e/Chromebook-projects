
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
        std::cerr << "The winsock dll not found" << std::endl;
        exit(1);
    }
    else {
        std::cout << "The winsock dll found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }

    // Create a socket
    SOCKET clientSocket;
    clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Check for successful socket creation
    if(clientSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }
    else {
        std::cout << "socket() successfully created" << std::endl;
    }


    // Connect to the server
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(DEFAULT_IPADDR);
    clientService.sin_port = htons(DEFAULT_PORT);

    // Use the connect function
    if(connect(clientSocket, reinterpret_cast<SOCKADDR*>(&clientService), sizeof(clientService)) == SOCKET_ERROR) {
        std::cerr << "Client: connect() failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }
    else {
        std::cout << "Client: connect() successful" << std::endl;
        std::cout << "Client: can start sending and receiving data.." << std::endl;
    }

    // Sending data to the server
    char sendBuffer[200];
    std::cout << "Enter the message: ";
    std::cin.getline(sendBuffer, 200);
    int sByteCount = send(clientSocket, sendBuffer, 200, 0);
    if(sByteCount == SOCKET_ERROR) {
        std::cerr << "Client send error: " << WSAGetLastError() << std::endl;
        exit(1);
    }
    else {
        std::cout << "Client: sent" << sByteCount << " bytes" << std::endl;
    }

    // Receive data from the server
    char receiveBuffer[200];
    int rByteCount = recv(clientSocket, receiveBuffer, 200, 0);
    if(rByteCount<0) {
        std::cerr << "Client recv error: " << WSAGetLastError() << std::endl;
        exit(1);
    }
    else {
        std::cout << "Client: Received data: " << receiveBuffer << std::endl;
    }

    return 0;
}