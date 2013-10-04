#ifndef __MULTIPLAYER_CHESS__
#define __MULTIPLAYER_CHESS__

#define LISTENING_PORT "7776"

int setup_socket(void);
bool socket_connect(int sockfd, char addr[]);
bool socket_bind(int sockfd);
int accept_connection(int sockfd);
void print_addr(struct sockaddr *addr);

#endif //__MULTIPLAYER_CHESS__