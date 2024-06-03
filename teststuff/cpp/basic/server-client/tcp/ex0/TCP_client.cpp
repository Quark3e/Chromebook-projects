
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
    for(;;) {
        bzero(buff, sizeof(buff)); // set buffer values to 0
        printf("Enter the string : ");
        n=0;
        while((buff[n++]=getchar()) != '\n'); // take input from the user
        write(sockfd, buff, sizeof(buff)); //write the buffer to sockfd
        bzero(buff, sizeof(buff)); // set buffer values to 0
        read(sockfd, buff, sizeof(buff));
        printf("From Server : %s", buff);
        if((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}


int main(int argc, char** argv) {
    int sockfd, connfd;
    std::string IP_addr = "81.229.133.253";
    
    struct sockaddr_in servaddr, cli;

    // socket creation and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd==-1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else printf("Socket successfully created\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.177");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        exit(0);
    }
    else printf("Connected to the server.\n");

    func(sockfd);

    close(sockfd);

    return 0;
}

