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

#include <chrono>
#include <thread>


#define DEFAULT__PORT   1089
#define DEFAULT__ADDR   "192.168.1.118"
#define DEFAULT__MSWAIT 33
#define MAXLINE 1024



// Driver code
int main(int argc, char** argv) {
    std::string serverAddr  = DEFAULT__ADDR;
    int serverPort          = DEFAULT__PORT;
    std::string defaultMsg  = "Hello";
    
    /**
     * different modes for running the client:
     * `0` - input message to cin and send that message to server, waiting for input.
     * `1` - read the stream from the server without concerning for any message from the client(this)
    */
    int mode = 0;

    if(argc>1) {
        for(int i=1; i<argc; i++) {
            if(strcmp(argv[i], "-p")==0 && i<argc-1) serverPort = std::stoi(argv[i+1]);
            if(strcmp(argv[i], "-a")==0 && i<argc-1) serverAddr = argv[i+1];
            if(strcmp(argv[i], "-m")==0 && i<argc-1) {
                mode = std::stoi(argv[i+1]);
            }
            if(strcmp(argv[i], "-R")==0 || strcmp(argv[i], "--READ")==0) {
                mode = 1;
            }
        }
        //serverAddr = argv[1];
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

    result = resolvehelper(serverAddr.c_str(), AF_INET, std::to_string(serverPort).c_str(), &addrDest);
    if(result!=0) {
        int lasterror = errno;
        std::cout << "error: " << lasterror;
        exit(1);
    }

	int n;
	socklen_t len;
    std::string msgRecev= "";
    std::string msgSend = "";
    while(true) {
        //std::cout << "from Client: ";
        //std::cin.clear();
        //std::cin.ignore();
        if(mode==0) {
            std::getline(std::cin, msgSend);
            if(msgSend=="exit") break;
        }
        else if(mode==1) msgSend = defaultMsg;

        result = sendto(
            sockfd,
            msgSend.c_str(),
            msgSend.length(),
            0,
            (sockaddr*)&addrDest,
            sizeof(addrDest)
        );
        if(mode==0) std::cout <<"\""<< msgSend <<"\" ["<<result<<" bytes sent.]"<<std::endl;
        
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
        if(n>0) {
        std::cout << "["<<n<<" bytes received.] ";
        std::cout << "from Server: " << buffer << std::endl;
        }

        if(mode!=0) std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT__MSWAIT));
        
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
