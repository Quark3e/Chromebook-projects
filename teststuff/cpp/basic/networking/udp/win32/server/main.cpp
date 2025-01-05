/**
 * @file main.cpp
 * @author sunmeat https://gist.github.com/sunmeat/02b60c8a3eaef3b8a0fb3c249d8686fd
 * @brief winsock2 udp server
 * @version 0.1
 * 
 */

#include <iostream>
#include <winsock2.h>
using namespace std;

#pragma comment(lib,"ws2_32.lib") // Winsock Library
#pragma warning(disable:4996) 

#define BUFLEN 512
#define PORT 8888

class UDPServer {
public:
    UDPServer() {
        // initialise winsock
        cout << "Initialising Winsock...\n";
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            cout << "Failed. Error Code: " << WSAGetLastError() << "\n";
            exit(0);
        }
        cout << "Initialised.\n";

        // create a socket
        if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
           cout << "Could not create socket: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }
        cout << "Socket created.\n";

        // prepare the sockaddr_in structure
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(PORT);

        // bind
        if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
            cout << "Bind failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }
        cout << "Bind done.\n";
    }

    ~UDPServer() {
        closesocket(server_socket);
        WSACleanup();
    }

    void start() {
        while (!exitRequested) {
            cout << "Waiting for data...\n";
            char message[BUFLEN] = {};

            // try to receive some data, this is a blocking call
            int message_len;
            int slen = sizeof(sockaddr_in);
            if ((message_len = recvfrom(server_socket, message, BUFLEN, 0, (sockaddr*)&client, &slen)) == SOCKET_ERROR) {
                cout << "recvfrom() failed with error code: " << WSAGetLastError() << "\n";
                exit(0);
            }

            // print details of the client/peer and the data received
            cout << "Received packet from " << inet_ntoa(client.sin_addr) << " " << ntohs(client.sin_port) << "\n";
            cout << "Data: " << message << "\n";

            cout << "Enter response (exit to stop server process): ";
            cin.getline(message, BUFLEN);

            // reply to the client with the same data
            if (sendto(server_socket, message, strlen(message), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR) {
                cout << "sendto() failed with error code: " << WSAGetLastError() << "\n";
                exit(EXIT_FAILURE);
            }

            if (strcmp(message, "exit") == 0) {
                cout << "Exiting server...\n";
                exitRequested = true;
                break;
            }
        }
    }

private:
    WSADATA wsa{};
    SOCKET server_socket = 0;
    sockaddr_in server{}, client{};
    bool exitRequested = false;
};

// int main() {
//     system("title UDP Server");

//     MoveWindow(GetConsoleWindow(), 650, 50, 500, 500, true);
  
//     UDPServer udpServer;
//     ShellExecuteA(GetConsoleWindow(), "open", "\"C:/Users/Alex/Desktop/ClientServer/x64/Debug/Client.exe\"", 0, 0, SW_NORMAL);

//     udpServer.start();
// }

int main() {
    system("title UDP Server");

    UDPServer udpServer;
    udpServer.start();
}