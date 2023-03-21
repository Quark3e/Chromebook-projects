// Client side implementation of UDP client-server model
#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 6969
#define MAXLINE 1024

// Driver code
int main() {
	int sockfd;
	char buffer[MAXLINE];
	const char *hello = "Hello from client";
	struct sockaddr_in	 servaddr;

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;
	
	int n;
	socklen_t len;
	
	sendto(sockfd, (const char *)hello, strlen(hello),
		MSG_CONFIRM, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
	std::cout<<"Hello message sent."<<std::endl;
		
	n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				MSG_WAITALL, (struct sockaddr *) &servaddr,
				&len);
	buffer[n] = '\0';
	std::cout<<"Server :"<<buffer<<std::endl;

	close(sockfd);
	return 0;
}



// #include <stdio.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <iostream>

// // #include "udpCOM_h1.h"

// #define PORT 4210

// using namespace std;

// int main(int argc, char *argv[]) {
//     int socket_desc;
//     struct sockaddr_in server;
//     char* message, server_reply[2000];

//     socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
//     if(socket_desc == -1) printf("Could not create socket.");

//     server.sin_family = AF_INET;
//     server.sin_addr.s_addr = INADDR_ANY;
//     server.sin_port = htons(PORT);

//     if(bind(socket_desc,(struct sockaddr*)&server, sizeof(server)) < 0) puts("bind failed");
//     puts("port bind done\n");


//     message = "Hello_World";
//     cout << strlen(message) << ": " << message << endl;
//     if(send(socket_desc, message, strlen(message), 0) < 0) {
//         puts("Send failed");
//         return 1;
//     }
//     puts("Data sent\n");

//     if(recv(socket_desc, server_reply, 2000, 0) < 0) {
//         puts("recv_failed");
//     }
//     puts("Reply received: ");
//     puts(server_reply);

//     return 0;
// }
