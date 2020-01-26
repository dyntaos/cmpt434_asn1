#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>

//#include <netinet/in.h>

#include "kv_network.h"


// Directly from beej's network guide:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int udp_init(char *port, struct addrinfo **ainfo) {
	struct addrinfo hints;
	struct addrinfo *servinfo, *p;
	int sockfd, rv;



	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
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

	*ainfo = p;
	freeaddrinfo(servinfo);

	return sockfd;
}

int udp_receive(int socket, void *buffer, size_t buffer_len, struct sockaddr *p, socklen_t *addr_len) {
	int recvlen;

	recvlen = recvfrom(socket, buffer, buffer_len, 0, p, addr_len);
	return recvlen;
}

int udp_send(int socket, void *buffer, size_t buffer_len, struct sockaddr *p) {
	int recvlen;

	recvlen = sendto(socket, buffer, buffer_len, 0, ((struct addrinfo*) p)->ai_addr, ((struct addrinfo*) p)->ai_addrlen);

	return recvlen;
}