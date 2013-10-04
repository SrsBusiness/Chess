#ifndef __MULTIPLAYER_CHESS__
#define __MULTIPLAYER_CHESS__

#define LISTENING_PORT "7776"

const char ping[] = "\x63\x68\x65\x73\x73";
const char pong[] = "\x50\x4c\x41\x59\x21";

struct addr_mask {
    unsigned long addr;
    unsigned long mask;
};

int setup_socket(void);
bool socket_connect(int sockfd, char addr[]);
bool socket_bind(int sockfd);
struct addr_mask active_interface(void);
int socket_search(void);
int accept_connection(int sockfd);
void print_addr(struct sockaddr *addr);

#endif //__MULTIPLAYER_CHESS__