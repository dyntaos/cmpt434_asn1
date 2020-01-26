#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define _IGNORE_PROTO_DEFINE
#include "kv_network.h"




int udp_client_init(char *host, char *port) {
	struct addrinfo hints;
	struct addrinfo *servinfo, *p;
	int sockfd, rv;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	if (host == NULL) hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (host == NULL && (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return -2;
	}

	freeaddrinfo(servinfo);

	return sockfd;
}


int udp_server_init(char *port) {
	return udp_client_init(NULL, port);
}


int udp_receive(int socket, void *buffer, size_t buffer_len) {
	int recvlen;

	recvlen = recv(socket, buffer, buffer_len, 0);
	return recvlen;
}


int udp_send(int socket, void *buffer, size_t buffer_len) {
	int recvlen;

	recvlen = send(socket, buffer, buffer_len, 0);
	return recvlen;
}