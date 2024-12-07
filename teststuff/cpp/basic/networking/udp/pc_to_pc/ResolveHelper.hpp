#pragma once
#ifndef HPP__RESOLVEHELPER_FUNC
#define HPP__RESOLVE_HELPER_FUNC

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
// #include <memory.h>
// #include <ifaddrs.h>
#include <net/if.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
// #include <poll.h>


inline int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr) {
    int result;
    addrinfo* result_list = NULL;
    addrinfo hints = {};
    hints.ai_family = family;

    hints.ai_socktype = SOCK_DGRAM;
    result = getaddrinfo(hostname, service, &hints, &result_list);
    if(result==0) {
        //ASSERT(result_list->ai_addrlen<=sieof(sockaddr_in));
        memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
        freeaddrinfo(result_list);
    }
    return result;
}

#endif
