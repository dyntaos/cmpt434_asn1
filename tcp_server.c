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

#include <kv_packet.h>
#include <kv_bintree.h>


#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10	 // how many pending connections queue will hold


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
	//struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	kv_binarytree *tree;
	char *k, *v;
	size_t malloc_len, string_len;
	ssize_t recv_bytes;
	kv_packet packet;


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

			if ((recv_bytes = recv(new_fd, &packet, sizeof(kv_packet), 0)) == -1) {
				perror("recv");
				exit(EXIT_FAILURE);
			}

			if (recv_bytes == 0) {
				break;
			}

			switch ((kv_message_command) packet.message_command) {


				case add:

					string_len = strlen(packet.key);
					malloc_len = string_len <= KV_MAX_STRING_LEN ? string_len + 1 : KV_MAX_STRING_LEN + 1;
					k = (char*) malloc(malloc_len);
					memcpy(k, packet.key, string_len);
					k[string_len] = 0;

					string_len = strlen(packet.value);
					malloc_len = string_len <= KV_MAX_STRING_LEN ? string_len + 1 : KV_MAX_STRING_LEN + 1;
					v = (char*) malloc(malloc_len);
					memcpy(v, packet.value, string_len);
					v[string_len] = 0;

					if (!add_kv_bintree(tree, k, v)) {
						//Failed to add to tree
						printf("Failed to add \"%s\":\"%s\"\n", k, v);

						if (!write_packet(&packet, (kv_message_command) packet.message_command, REPLY, 0, 0, NULL, NULL)) {
							fprintf(stderr, "Error: Invalid arguments to add reply!\n");
							free(k);
							free(v);
							k = v = NULL;
							continue;
						}

						free(k);
						free(v);

					} else {
						printf("Added \"%s\":\"%s\"\n", k, v);
						packet.message_type = REPLY;
					}

					if (send(new_fd, &packet, sizeof(kv_packet), 0) == -1) {
						perror("Error: Failed to send add command!");
					}

					k = v = NULL;

					break;


				case get_value:

					if (get_kv_bintree(tree, packet.key, (void**) &v)) {
						printf("Get value: \"%s\":\"%s\"\n", packet.key, v);

						if (!write_packet(&packet, get_value, REPLY, 1, 1, packet.key, v)) {
							// Execution should never reach here
							fprintf(stderr, "Error: Invalid arguments to get_value reply!\n");
						}

					} else {
						printf("Get value: \"%s\": Key does not exist...\n", packet.key);

						if (!write_packet(&packet, get_value, REPLY, 0, 0, NULL, NULL)) {
							// Execution should never reach here
							fprintf(stderr, "Error: Invalid arguments to get_value error reply!\n");
						}
					}

					if (send(new_fd, &packet, sizeof(kv_packet), 0) == -1) {
						perror("Error: Failed to send get_value reply!");
					}

					v = NULL;

					break;


				case get_all:

					if (size_kv_bintree(tree) == 0) {
						if (!write_packet(&packet, get_all, REPLY, 0, 0, NULL, NULL)) {
							// Execution should never reach here
							fprintf(stderr, "Error: Invalid arguments to get_all reply!\n");
						}

						printf("Getall: 0 items\n");

						if (send(new_fd, &packet, sizeof(kv_packet), 0) == -1) {
							perror("Error: Failed to send get_value reply!");
						}

					} else {
						size_t i = 1;

						printf("Getall: %u items\n", size_kv_bintree(tree));
						cursor_init_kv_bintree(tree);

						do {
							if (!cursor_get_kv_bintree(tree, &k, (void**) &v)) break;

							if (!write_packet(&packet, get_all, REPLY, size_kv_bintree(tree), i, k, v)) {
								// Execution should never reach here
								fprintf(stderr, "Error: Invalid arguments to get_all iterator reply!\n");
							}

							printf("%lu  \"%s\":\"%s\"\n", i, k, v);

							if (send(new_fd, &packet, sizeof(kv_packet), 0) == -1) {
								perror("Error: Failed to send get_value iterator reply!");
							}

							i++;

						} while(cursor_next_kv_bintree(tree));

						k = v = NULL;
					}

					break;


				case remove_cmd:

					if ((v = remove_kv_bintree(tree, packet.key, free)) != NULL) {
						printf("Remove key: \"%s\":\"%s\"\n", packet.key, v);

						if (!write_packet(&packet, remove_cmd, REPLY, 1, 1, packet.key, v)) {
							// Execution should never reach here
							fprintf(stderr, "Error: Invalid arguments to remove reply!\n");
							continue;
						}

					} else {
						printf("Remove key: \"%s\": Key does not exist...\n", packet.key);

						if (!write_packet(&packet, remove_cmd, REPLY, 0, 0, NULL, NULL)) {
							// Execution should never reach here
							fprintf(stderr, "Error: Invalid arguments to remove error reply!\n");
							continue;
						}
					}

					if (send(new_fd, &packet, sizeof(kv_packet), 0) == -1) {
						perror("Error: Failed to send remove reply!");
					}

					free(v);
					v = NULL;

					break;


				case quit:

					printf("Client disconnected...\n");
					quit_flag = 1;
					break;


				case invalid:
				default:
					quit_flag = 1;

			}

			if (quit_flag) {
				break;
			}
			print_in_order_kv_bintree(tree);
		}

		close(new_fd);

	}

	return 0;
}

