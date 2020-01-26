#ifndef _TCP_H
#define _TCP_H

#include <sys/socket.h>
#include <netdb.h>

#define BACKLOG 10


void *get_in_addr(struct sockaddr *sa);

int tcp_init(char *port);
int tcp_accept(int sock_fd);
int tcp_receive(int socket, void *buffer, size_t buffer_len);
int tcp_send(int socket, void *buffer, size_t buffer_len);

int udp_init(char *port, struct addrinfo **ainfo);
int udp_receive(int socket, void *buffer, size_t buffer_len, struct sockaddr *p, socklen_t *addr_len);
int udp_send(int socket, void *buffer, size_t buffer_len, struct sockaddr *p);


#endif //_TCP_H