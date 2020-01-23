/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <kv_server_packet.h>
#include <kv_bintree.h>


#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10	 // how many pending connections queue will hold



void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}



// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints;
	struct addrinfo *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	kv_binarytree *tree;
	char *k, *v;
	size_t malloc_len, string_len;
	int32_t recv_bytes;
	kv_packet received_packet;


	(void) argc;
	(void) argv;

	tree = alloc_kv_bintree();

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
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

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("Server: Waiting for connections...\n");

	for (;;) {
		int quit_flag = 0;

		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr*) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr*) &their_addr), s, sizeof(s));
		printf("Server: Got connection from %s\n", s);

		for (;;) {

			if ((recv_bytes = recv(new_fd, &received_packet, sizeof(kv_packet), 0)) == -1) {
				perror("recv");
				exit(EXIT_FAILURE);
			}

			if (recv_bytes == 0 || received_packet.message_command == quit) {
				break;
			}

			switch (received_packet.message_command) {
				case add:

					string_len = strlen(received_packet.key);
					malloc_len = string_len <= KV_MAX_STRING_LEN ? string_len + 1 : KV_MAX_STRING_LEN + 1;
					k = (char*) malloc(malloc_len);
					memcpy(k, received_packet.key, string_len);
					k[string_len] = 0;

					string_len = strlen(received_packet.value);
					malloc_len = string_len <= KV_MAX_STRING_LEN ? string_len + 1 : KV_MAX_STRING_LEN + 1;
					v = (char*) malloc(malloc_len);
					memcpy(v, received_packet.value, string_len);
					v[string_len] = 0;

					if (!add_kv_bintree(tree, k, v)) {
						//Failed to add to tree
						//TODO
						printf("Failed to add \"%s\":\"%s\"\n", k, v);

						free(k);
						free(v);
						k = v = NULL;
						continue;
					}

					printf("Added \"%s\":\"%s\"\n", k, v);
					// TODO: Reply to client

					k = v = NULL;

					break;
				case get_value:

					if (get_kv_bintree(tree, received_packet.key, (void**) &v)) {
						printf("Get value: \"%s\":\"%s\"\n", received_packet.key, v);
					} else {
						printf("Get value: \"%s\": Key does not exist...\n", received_packet.key);
					}
					// TODO: Reply to client

					v = NULL;

					break;
				case get_all:
					// TODO

					break;
				case remove_cmd:

					if ((v = remove_kv_bintree(tree, received_packet.key, free)) != NULL) {
						printf("Remove key: \"%s\":\"%s\"\n", received_packet.key, v);
					} else {
						printf("Remove key: \"%s\": Key does not exist...\n", received_packet.key);
					}

					// TODO: Reply to client

					free(v);
					v = NULL;

					break;
				case quit:
				case invalid:
				default:
					quit_flag = 1;
			}
			if (quit_flag) {
				break;
			}
			printf("Size of tree: %d\n", size_kv_bintree(tree));
			//print_in_order_kv_bintree(tree);
		}

		close(new_fd);

	}

	return 0;
}

