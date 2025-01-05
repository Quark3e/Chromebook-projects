/**
 * @file main.cpp
 * @author sunmeat https://gist.github.com/sunmeat/02b60c8a3eaef3b8a0fb3c249d8686fd
 * @brief winsock2 udp client
 * @version 0.1
 * 
 */

#include <iostream>
#include <winsock2.h>
using namespace std;

#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996) 

// #define SERVER "127.0.0.1"  // or "localhost" - ip address of UDP server
#define SERVER "192.168.1.117"
#define BUFLEN 512  // max length of answer
// #define PORT 8888  // the port on which to listen for incoming data
#define PORT 1089

class UDPClient {
public:
    UDPClient() {
        // initialise winsock
        cout << "Initialising Winsock...\n";
        if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
            cout << "Failed. Error Code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }
        cout << "Initialised.\n";

        // create socket
        if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
            cout << "socket() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }

        // setup address structure
        memset((char*)&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr.S_un.S_addr = inet_addr(SERVER);
    }

    ~UDPClient() {
        closesocket(client_socket);
        WSACleanup();
    }

    void start() {
        while (true) {
            char message[BUFLEN];
            cout << "Enter message: ";
            cin.getline(message, BUFLEN);

            // send the message
            if (sendto(client_socket, message, strlen(message), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR) {
                cout << "sendto() failed with error code: " << WSAGetLastError() << "\n";
                exit(EXIT_FAILURE);
            }

            // receive a reply and print it
            // clear the answer by filling null, it might have previously received data
            char answer[BUFLEN] = {};

            // try to receive some data, this is a blocking call
            int slen = sizeof(sockaddr_in);
            int answer_length;
            if ((answer_length = recvfrom(client_socket, answer, BUFLEN, 0, (sockaddr*)&server, &slen)) == SOCKET_ERROR) {
                cout << "recvfrom() failed with error code: " << WSAGetLastError() << "\n";
                exit(EXIT_FAILURE);
            }

            cout << "Server's response: " << answer << "\n";
        }
    }

private:
    WSADATA ws;
    SOCKET client_socket;
    sockaddr_in server;
};

int main() {
    system("title UDP Client");

    MoveWindow(GetConsoleWindow(), 50, 50, 500, 500, true);

    UDPClient udpClient;
    udpClient.start();
}
