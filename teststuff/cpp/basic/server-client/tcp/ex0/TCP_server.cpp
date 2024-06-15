
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#define MAX     80
#define PORT    2225
#define SA struct sockaddr

bool runServer = false;

void func(int connfd) {
    char buff_send[MAX];
    char buff_recev[MAX];

    int n;
    
    int return_read = 0;
    int return_select = 0;

    while(true) {
        bzero(buff_recev, MAX);

        std::cout << "[server] Called read()"<<std::endl;
        return_read = read(connfd, buff_recev, sizeof(buff_recev));

        if(return_read<0) {
            printf("[server] read() returned -1. Closing connection\n");
            runServer = true;
            break;
        }
        if(strncmp("$shutdown", buff_recev, 9)==0) {
            printf("[server] \"$shutdown\" called. Closing server..\n");
            runServer = false;
            break;
        }
        else if(strncmp("$close", buff_recev, 6)==0) {
            printf("[server] \"$close\" called. Closing connection..\n");
            runServer = true;
            break;
        }

        printf(" >> From client: \"%s\"To client : ", buff_recev);
        bzero(buff_send, MAX);
        n=0;

        while((buff_send[n++]=getchar()) != '\n');

        if(write(connfd, buff_send, sizeof(buff_send))<0) {
            printf("[server] write() returned <0. Closing session..\n");
            break;
        }

        if(strncmp("exit", buff_send, 4) == 0) {
            printf("[server] server Exit...\n");
            runServer = false;
            break;
        }
        else if(strncmp("close", buff_send, 5) == 0) {
            printf("[server] server closing...\n");
            runServer = true;
            break;
        }
    }
}


int main(int argc, char** argv) {
    unsigned int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket creation and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd==-1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign iP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // binding newly created socket to given IP and verification
    if((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else printf("Socket successfully bound..\n");

    // server is ready to listen and verification
    if((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else printf("Server listening..\n");
    len = sizeof(cli);

    runServer = true;
    while(runServer) {
        // accept the data packet from client and verification
        connfd = accept(sockfd, (SA*)&cli, &len);
        if(connfd < 0) {
            printf("server accept failed...\n");
            exit(0);
        }
        else std::cout<<"server accepted the client: "<<sockfd<<", "<<connfd<<"\n";
        
        func(connfd);

        std::cout << "server: connection closing: "<<close(sockfd)<<", "<<close(connfd)<<std::endl;
    }
    return 0;
}
