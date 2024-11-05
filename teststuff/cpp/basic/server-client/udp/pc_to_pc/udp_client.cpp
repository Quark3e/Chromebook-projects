// Client side implementation of UDP client-server model 
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <fcntl.h>
#include <iostream>
#include <string>

#include "ResolveHelper.hpp"

#define PORT    8080
#define ADDR    INADDR_ANY
#define MAXLINE 1024



// Driver code
int main(int argc, char** argv) {
    std::string serverAddr = "";
    if(argc>1) {
        serverAddr = argv[1];
    }

    int result = 0;
	int sockfd;
	char buffer[MAXLINE];
	//const char *hello = "Hello from client";
	struct sockaddr_in servaddr = {};
    sockaddr_storage addrDest = {};

	// Creating socket file descriptor 
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

	memset(&addrDest, 0, sizeof(addrDest));

	// Filling server information
	servaddr.sin_family = AF_INET;
	//servaddr.sin_port = htons(PORT);
    //addrDest.sin_family = AF_INET;
    //addrDest.sin_port = htons(PORT);
    result = bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
    if(result==-1) {
        int lasterror = errno;
        std::cout << "error: " << lasterror;
        exit(1);
    }

    if(argc<=1 && serverAddr.length()==0) {
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        servaddr.sin_addr.s_addr = INADDR_ANY;
    }
    else {
        result = resolvehelper(serverAddr.c_str(), AF_INET, std::to_string(PORT).c_str(), &addrDest);
        if(result!=0) {
            int lasterror = errno;
            std::cout << "error: " << lasterror;
            exit(1);
        }
    }

	int n;
	socklen_t len;
    std::string msgRecev= "";
    std::string msgSend = "";
    while(true) {
        std::cout << "from Client: ";
        //std::cin.clear();
        //std::cin.ignore();
        std::getline(std::cin, msgSend);
        if(msgSend=="exit") break;
        result = sendto(
            sockfd,
            msgSend.c_str(),
            msgSend.length(),
            0,
            (sockaddr*)&addrDest,
            sizeof(addrDest)
        );
        std::cout <<"\""<< msgSend <<"\" ["<<result<<" bytes sent.]"<<std::endl;
        
        socklen_t len;
        int n = recvfrom(
            sockfd,
            (char*)buffer,
            MAXLINE,
            MSG_WAITALL,
            (struct sockaddr*)&addrDest,
            &len
        );
        buffer[n] = '\0';
        std::cout << "["<<n<<" bytes received.] ";
        std::cout << "from Server: " << buffer << std::endl;
    }
    /*
	sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
	std::cout<<"Hello message sent."<<std::endl;
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
	buffer[n] = '\0';
	std::cout<<"Server :"<<buffer<<std::endl; 
    */
	close(sockfd); 
	return 0; 
}
