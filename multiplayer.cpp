#include "multiplayer.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

struct addrinfo hints, *res;

int setup_socket(void) {
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, LISTENING_PORT, &hints, &res) == -1) {
        return 0;
    }
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    int yes=1;
    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        return 0;
    }
    return sockfd;
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
