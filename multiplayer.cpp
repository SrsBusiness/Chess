#include "multiplayer.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct addrinfo hints, *res;

int setup_socket(void) {
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, LISTENING_PORT, &hints, &res) == -1) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    int yes=1;
    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

unsigned long long active_interface(void) {
    struct ifaddrs *ifaddr, *ifa;
    int family, flags;
    unsigned long long res = 0;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;
        flags = ifa->ifa_flags;

        if (family == AF_INET && (flags&IFF_LOOPBACK) == 0) {
            res = ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr.s_addr << sizeof(long);
            res |= ((struct sockaddr_in*)ifa->ifa_netmask)->sin_addr.s_addr;
        }
    }
    freeifaddrs(ifaddr);
    return res;
}

int socket_search(void) {
    unsigned long long iflong = active_interface();
    unsigned long laddr = iflong >> 32, naddr = (unsigned long)iflong;
    laddr &= naddr;
    unsigned int n = 0;
    while (((naddr >> n)&1 == 0) && (n <= 32)) {
        n++;
    }

    char addr[INET_ADDRSTRLEN];
    int sockfd;
    char str[6];
    for (int i = 0; i < (1<<n); i++) {
        struct in_addr nladdr = {laddr+i};
        inet_ntop(AF_INET, &(nladdr), addr, INET_ADDRSTRLEN);

        if ((sockfd = setup_socket()) < 0) {
            fprintf(stderr, "setup_socket: unable to create socket\n");
            return 2;
        }
        if (socket_connect(sockfd, addr) == false) {
            continue;
        }
        memset(str, 0, 6 * sizeof(char));
        send(sockfd, ping, 6, 0);
        recv(sockfd, str, 6, 0);
        if (strcmp(str, pong) == 0) {
            return sockfd;
        }
        close(sockfd);
    }
    return -1;
}

bool socket_connect(int sockfd, char addr[]) {
    if (getaddrinfo(addr, LISTENING_PORT, &hints, &res) == -1) {
        return false;
    }
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        return false;
    }
    return true;
}

bool socket_bind(int sockfd) {
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        return false;
    }
    if (listen(sockfd, 1) == -1) {
        return false;
    }
    return true;
}

int accept_connection(int sockfd) {
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof their_addr;
    int fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    return fd;
}
