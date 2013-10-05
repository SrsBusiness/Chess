#include "multiplayer.h"
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
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

struct addr_mask active_interface(void) {
    struct ifaddrs *ifaddr, *ifa;
    int family, flags;
    struct addr_mask res = {0, 0};

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
            unsigned long addr = ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr.s_addr;
            unsigned long netmask = ((struct sockaddr_in*)ifa->ifa_netmask)->sin_addr.s_addr;
            res.addr = addr;
            res.mask = netmask;
        }
    }
    freeifaddrs(ifaddr);
    return res;
}

int socket_search(void) {
    struct addr_mask iflong = active_interface();
    unsigned long laddr = htonl(iflong.addr);
    unsigned long naddr = htonl(iflong.mask);
    unsigned long lanaddr = laddr & naddr;
    unsigned int n = 0;
    while ((((naddr >> n)&1) == 0) && (n <= 32)) {
        n++;
    }
    char addr[INET_ADDRSTRLEN];
    int sockfd;
    char str[6];
    for (int i = 0; i < (1<<n); i++) {
        struct in_addr nladdr = {ntohl(lanaddr+i)};
        inet_ntop(AF_INET, &(nladdr), addr, INET_ADDRSTRLEN);
        sockfd = setup_socket();
        if (socket_connect(sockfd, addr, 100000) == true) {
            memset(str, 0, 6 * sizeof(char));
            send(sockfd, ping, 6, 0);
            recv(sockfd, str, 6, 0);
            if (strcmp(str, pong) == 0) {
                return sockfd;
            }
        }
        close(sockfd);
    }
    return -1;
}

bool socket_connect(int sockfd, char addr[], int usecs) {
    int n, error, flags;
    socklen_t len;
    fd_set rset, wset;
    struct timeval tval;
    if (getaddrinfo(addr, LISTENING_PORT, &hints, &res) == -1) {
        return false;
    }
    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    if ((n = connect(sockfd, res->ai_addr, res->ai_addrlen)) < 0) {
        if (errno != EINPROGRESS) {
            return false;
        }
    }
    if (n == 0) {
        fcntl(sockfd, F_SETFL, O_ASYNC);
        return true;
    }
    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset;
    tval.tv_sec = 0;
    tval.tv_usec = usecs;
    if ((n = select(sockfd+1, &rset, &wset, NULL, &tval)) == 0) {
        errno = ETIMEDOUT;
        return false;
    }
    if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
        len = sizeof(error);
        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
            return false;
    } else {
        return false;
    }
    fcntl(sockfd, F_SETFL, flags);
    if (error) {
        errno = error;
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
