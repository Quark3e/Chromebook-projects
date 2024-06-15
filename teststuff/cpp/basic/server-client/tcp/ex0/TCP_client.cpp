
#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX     80
#define PORT    2225 //8080
#define SA struct sockaddr


void func(int sockfd) {
    char buff[MAX];
    int n;
    int write_bytes = 0;
    int read_bytes = 0;
    while(true) {
        bzero(buff, sizeof(buff)); // set buffer values to 0
        printf("Enter the string : ");
        n=0;
        while((buff[n++]=getchar()) != '\n'); // take input from the user
        
        std::cout << "write(): "<<write(sockfd, buff, sizeof(buff))<<"\n";
        // if(write(sockfd, buff, sizeof(buff))<0) {//write the buffer to sockfd
        //     printf("[client] write() returned <0. closing session.");
        //     break;
        // }
        bzero(buff, sizeof(buff)); // set buffer values to 0
        if(read(sockfd, buff, sizeof(buff))<0) {
            printf("[client] connection error: exiting session\n");
            break;
        } 
        printf("From Server : %s", buff);
        if((strncmp(buff, "$exit", 5)) == 0) {
            printf("[client] \"$exit\" received from server. closing client\n");
            break;
        }
    }
}


int main(int argc, char** argv) {
    int sockfd, connfd;
    std::string IP_addr = "81.229.133.253";
    IP_addr = "192.168.1.177";

    struct sockaddr_in servaddr, cli;

    // socket creation and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd==-1) {
        printf("[client] socket creation failed...\n");
        exit(0);
    }
    else printf("[client] Socket successfully created\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(IP_addr.c_str());
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("[client] Connection with the server failed...\n");
        exit(0);
    }
    else printf("[client] Connected to the server.\n");

    func(sockfd);

    close(sockfd);

    return 0;
}

