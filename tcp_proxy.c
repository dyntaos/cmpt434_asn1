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


void proxy_string_modifier(char *s) {
	size_t char_count = 0;
	int i;

	for (i = 0; i <= KV_MAX_STRING_LEN && s[i] != 0; i++) {
		if (s[i] == 'c' || s[i] == 'm' || s[i] == 'p' || s[i] == 't') {
			char_count++;
		}
	}

	s[i + char_count] = 0;

	for (i += char_count - 1; i >= 0; i--) {
		if (s[i - char_count] == 'c' || s[i - char_count] == 'm' || s[i - char_count] == 'p' || s[i - char_count] == 't') {
			s[i] = s[i - 1] = s[i - char_count];
			char_count--;
			i--;
			continue;
		}
		s[i] = s[i - char_count];
	}
}


int main(int argc, char *argv[]) {
	int client_listen_fd, client_connection_fd;
	struct addrinfo hints, *servinfo, *client_p;
	struct sockaddr_storage client_addr;
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char server_s[INET6_ADDRSTRLEN], client_s[INET6_ADDRSTRLEN];
	int rv;

	int server_fd;
	struct addrinfo *server_p;

	char *k, *v;
	size_t malloc_len, string_len;
	ssize_t recv_bytes;
	kv_packet packet;


	(void) argc;
	(void) argv;


	/*****************************************
	 *     Prepare connection to server      *
	 *****************************************/

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return EXIT_FAILURE;
	}

	for (server_p = servinfo; server_p != NULL; server_p = server_p->ai_next) {
		if ((server_fd = socket(server_p->ai_family, server_p->ai_socktype, server_p->ai_protocol)) == -1) {
			perror("Proxy: Error opening socket for server connection");
			continue;
		}

		if (connect(server_fd, server_p->ai_addr, server_p->ai_addrlen) == -1) {
			perror("Proxy: Error connecting to server");
			close(server_fd);
			continue;
		}

		break;
	}

	if (server_p == NULL) {
		fprintf(stderr, "Proxy: Failed to connect to server\n");
		return 2;
	}

	inet_ntop(server_p->ai_family, get_in_addr((struct sockaddr *)server_p->ai_addr), server_s, sizeof(server_s));
	printf("Proxy: Connected to server at %s\n", server_s);

	freeaddrinfo(servinfo);




	/*****************************************
	 *     Prepare connection to client      *
	 *****************************************/

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for (client_p = servinfo; client_p != NULL; client_p = client_p->ai_next) {
		if ((client_listen_fd = socket(
				client_p->ai_family,
				client_p->ai_socktype,
				client_p->ai_protocol
		)) == -1) {
			perror("Proxy: Failed to open socket to listen for client");
			continue;
		}

		if (setsockopt(
				client_listen_fd,
				SOL_SOCKET,
				SO_REUSEADDR,
				&yes,
				sizeof(int)
		) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(client_listen_fd, client_p->ai_addr, client_p->ai_addrlen) == -1) {
			close(client_listen_fd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);

	if (client_p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(client_listen_fd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	printf("Proxy: Waiting for connections...\n");




	for (;;) {
		int quit_flag = 0;

		sin_size = sizeof client_addr;
		client_connection_fd = accept(client_listen_fd, (struct sockaddr*) &client_addr, &sin_size);
		if (client_connection_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr*) &client_addr), client_s, sizeof(client_s));
		printf("Proxy: Got connection from client at %s\n", client_s);

		for (;;) {

			if ((recv_bytes = recv(client_connection_fd, &packet, sizeof(kv_packet), 0)) == -1) {
				perror("recv");
				exit(EXIT_FAILURE);
			}

			if (recv_bytes == 0) {
				printf("Client connection closed...\n");
				break;
			}

			if (send(server_fd, &packet, sizeof(kv_packet), 0) == -1) {
				perror("Error: Failed to forward packet!");
				continue; // TODO: Is this needed?
			}


			do {
				if ((recv_bytes = recv(server_fd, &packet, sizeof(kv_packet), 0)) == -1) {
					perror("recv"); // TODO
					exit(EXIT_FAILURE);
				}

				if (recv_bytes == 0) {
					printf("Client connection closed...\n");
					break;
				}

				proxy_string_modifier(packet.value);

				if (send(server_fd, &packet, sizeof(kv_packet), 0) == -1) {
					perror("Error: Failed to forward packet!");
					continue; // TODO: Is this needed?
				}

			} while(ntohs(packet.kv_pair_number) != htons(packet.kv_pairs_total));

			if (((kv_message_command) packet.message_command) == quit) {
				printf("Client disconnected from proxy\n");
				break;
			}
		}

		close(client_connection_fd);

	}

	return 0;
}

