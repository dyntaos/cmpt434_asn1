#ifndef _TCP_H
#define _TCP_H

#include <sys/socket.h>
#include <netdb.h>

#define BACKLOG 10


int tcp_client_init(char *host, char *port);
int tcp_server_init(char *port);
int tcp_accept(int sock_fd);
int tcp_receive(int socket, void *buffer, size_t buffer_len);
int tcp_send(int socket, void *buffer, size_t buffer_len);

int udp_client_init(char *host, char *port, struct addrinfo **ainfo);
int udp_server_init(char *port, struct addrinfo **ainfo);
int udp_receive(int socket, void *buffer, size_t buffer_len, struct sockaddr *p, socklen_t *addr_len);
int udp_send(int socket, void *buffer, size_t buffer_len, struct sockaddr *p, socklen_t addr_len);


#ifdef _TCP

#define		SOCKET_CLIENT_INIT(host, port, addrinfo)					tcp_client_init(host, port)
#define		SOCKET_SERVER_INIT(port, addrinfo)							tcp_server_init(port)
#define		SOCKET_ACCEPT(socket_fd)									tcp_accept(socket_fd)
#define		SOCKET_RECEIVE(socket, buffer, buffer_len, p, addr_len)		tcp_receive(socket, buffer, buffer_len)
#define		SOCKET_SEND(socket, buffer, buffer_len, p, addr_len)		tcp_send(socket, buffer, buffer_len)

#else

#ifdef _UDP

#define		SOCKET_CLIENT_INIT(host, port, addrinfo)					udp_client_init(host, port, addrinfo)
#define		SOCKET_SERVER_INIT(port, addrinfo)							udp_server_init(port, addrinfo)
#define		SOCKET_ACCEPT(socket_fd)									socket_fd
#define		SOCKET_RECEIVE(socket, buffer, buffer_len, p, addr_len)		udp_receive(socket, buffer, buffer_len, p, addr_len)
#define		SOCKET_SEND(socket, buffer, buffer_len, p, addr_len)		udp_send(socket, buffer, buffer_len, p, addr_len)

#else

#ifndef _IGNORE_PROTO_DEFINE
#error Define "TCP" or "UDP" when compiling
#endif // _IGNORE_PROTO_DEFINE

#endif // _UDP
#endif // _TCP

#endif //_TCP_H