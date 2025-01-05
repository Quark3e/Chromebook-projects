
//WORKS

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include <poll.h>

#include <useful.hpp>

#define MAXLINE 2048

int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr)
{
    int result;
    addrinfo* result_list = NULL;
    addrinfo hints = {};
    hints.ai_family = family;
    hints.ai_socktype = SOCK_DGRAM; // without this flag, getaddrinfo will return 3x the number of addresses (one for each socket type).
    result = getaddrinfo(hostname, service, &hints, &result_list);
    if (result == 0)
    {
        //ASSERT(result_list->ai_addrlen <= sizeof(sockaddr_in));
        memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
        freeaddrinfo(result_list);
    }

    return result;
}



int main() {
    int result = 0;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);
    char buffer[MAXLINE];
    const char* PORT = "53";
    const char* IPADDR = "192.168.1.117"; //"192.168.1.231" - other board


    char szIP[100];

    sockaddr_in addrListen = {}; // zero-int, sin_port is 0, which picks a random port for bind.
    addrListen.sin_family = AF_INET;
    result = bind(sock, (sockaddr*)&addrListen, sizeof(addrListen));
    if (result == -1) {
       int lasterror = errno;
       std::cout << "error: " << lasterror;
       exit(1);
    }

    sockaddr_storage addrDest = {};
    result = resolvehelper(IPADDR, AF_INET, PORT, &addrDest);
    if (result != 0) {
       int lasterror = errno;
       std::cout << "error: " << lasterror;
       exit(1);
    }
    

    const char* msg = "1";
    while(true) {
        usleep(10'000);
        clock_t t1 = clock();
        size_t msg_length = strlen(msg);
        result = sendto(sock, msg, msg_length, 0, (sockaddr*)&addrDest, sizeof(addrDest));

        // std::cout << result << " bytes sent" << std::endl;
        printf("%d bytes sent.\t", result);
        socklen_t len;

        int n = recvfrom(sock, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &addrDest, &len);
        buffer[n] = '\0';
        std::cout << buffer[n-1] << "\t";
        // std::cout<<"Server :"<<buffer<<std::endl;
        printf("Server: \"%s\"\n", buffer);

    }
    return 0;

}
