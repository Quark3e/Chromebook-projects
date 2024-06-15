
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
    char buff[MAX];
    int n;
    
    int return_read = 0;
    int return_select = 0;

    fd_set rfds;

    FD_ZERO(&rfds);
    FD_SET(connfd, &rfds);


    struct timeval tv;
    tv.tv_usec  = 0.0;
    tv.tv_sec   = 0.0;

    while(true) {
        bzero(buff, MAX);

        // fd_set rsd = rfds;

        // return_select = select(connfd+1, &rfds, 0, 0, &tv);

        // if(return_select<0 || (return_select==0 && !(tv.tv_sec==0 && tv.tv_usec==0))) {
        //     std::cout << "ERROR: ";
        //     if(return_select<0)         std::cout << "select() return -1.";
        //     else if(return_select==0)   std::cout << "select() timeout.";
        //     std::cout << " Closing connection.."<<std::endl;
        //     break;
        // }
        // else if(return_select==0) continue;

        return_read = read(connfd, buff, sizeof(buff));

        if(return_read<=0) {
            std::cout << "ERROR: ";
            if(return_read==0)      std::cout << "read() returned 0";
            else if(return_read<0)  std::cout << "read() returned -1";
            std::cout << " Closing connection.."<<std::endl;
            break;
        }

        printf("From client: %s\t To client : ", buff);
        bzero(buff, MAX);
        n=0;

        while((buff[n++]=getchar()) != '\n');

        write(connfd, buff, sizeof(buff));

        if(strncmp("exit", buff, 4) == 0) {
            printf("Client Exit...\n");
            break;
        }
        else if(strncmp("close", buff, 5) == 0) {
            printf("Server closing...\n");
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
        else printf("server accepted the client...\n");
        
        func(connfd);

        close(sockfd);
    }
    return 0;
}
