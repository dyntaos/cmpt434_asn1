#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define _IGNORE_PROTO_DEFINE
#include "kv_network.h"


// TODO: Audit all string messages



int tcp_client_init(char *host, char *port) {
	int rv, fd;
	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			//perror("Error opening socket for server connection");
			continue;
		}

		if (connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
			//perror("Error connecting to server");
			close(fd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "Failed to connect to server\n");
		return -2;
	}

	printf("Connected...\n");

	freeaddrinfo(servinfo);
	return fd;
}


int tcp_server_init(char *port) {
	struct addrinfo hints;
	struct addrinfo *servinfo, *p;
	int sockfd, rv, yes = 1;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(
				p->ai_family,
				p->ai_socktype,
				p->ai_protocol
		)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(
				sockfd,
				SOL_SOCKET,
				SO_REUSEADDR,
				&yes,
				sizeof(int)
		) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	return sockfd;
}


int tcp_accept(int sock_fd) {
	socklen_t sin_size;
	struct sockaddr_storage their_addr;
	int new_fd;

	sin_size = sizeof(their_addr);
	new_fd = accept(sock_fd, (struct sockaddr*) &their_addr, &sin_size);
	if (new_fd == -1) {
		perror("Failed to accept incoming connection");
		return -1;
	}

	return new_fd;
}


int tcp_receive(int socket, void *buffer, size_t buffer_len) {
	return recv(socket, buffer, buffer_len, 0);
}


int tcp_send(int socket, void *buffer, size_t buffer_len) {
	return send(socket, buffer, buffer_len, 0);
}