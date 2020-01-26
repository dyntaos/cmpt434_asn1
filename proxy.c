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

#include <kv_network.h>
#include <kv_packet.h>




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
	int client_listen_fd, client_connection_fd, server_fd;
	//struct addrinfo hints, *servinfo, *client_p, *server_p;
	struct addrinfo *server_p;
	//struct sockaddr_storage client_addr;
	//socklen_t sin_size;
	int recv_bytes;
	kv_packet packet;


	(void) argc;
	(void) argv;
	(void) server_p;

	/*****************************************
	 *     Prepare connection to server      *
	 *****************************************/

	server_fd = SOCKET_CLIENT_INIT(argv[2], argv[3], &server_p);
	if (server_fd <= 0) {
		fprintf(stderr, "Failed to open connection to server!\n");
		exit(EXIT_FAILURE);
	}

	printf("Proxy: Connected to server...\n");


	/*****************************************
	 *     Prepare connection to client      *
	 *****************************************/

	client_listen_fd = tcp_server_init(argv[1]);
	if (client_listen_fd <= 0) {
		fprintf(stderr, "Failed to open socket to listen for client connections!\n");
		exit(EXIT_FAILURE);
	}

	printf("Proxy: Waiting for connections...\n");



	for (;;) {

		//sin_size = sizeof client_addr;
		client_connection_fd = tcp_accept(client_listen_fd);
		if (client_connection_fd == -1) {
			perror("accept");
			continue;
		}

		printf("Proxy: Got connection from client\n");

		for (;;) {

			if ((recv_bytes = tcp_receive(client_connection_fd, &packet, sizeof(kv_packet))) == -1) {
				perror("recv"); // todo
				exit(EXIT_FAILURE);
			}

			if (recv_bytes == 0) {
				printf("Client connection closed...\n");
				break;
			}

			printf("Got packet from client\n");

			if (SOCKET_SEND(server_fd, &packet, sizeof(kv_packet), ((struct addrinfo*) server_p)->ai_addr, ((struct addrinfo*) server_p)->ai_addrlen) == -1) {
				perror("Error: Failed to forward packet!");
				continue; // TODO: Is this needed?
			}

			printf("Forwarded packet from client to server\n");

			do {
				if ((recv_bytes = SOCKET_RECEIVE(server_fd, &packet, sizeof(kv_packet), NULL, NULL)) == -1) {
					perror("recv"); // TODO
					exit(EXIT_FAILURE);
				}

				if (recv_bytes == 0) {
					printf("Client connection closed...\n");
					break;
				}

				printf("Got reply packet from server (%u of %u)\n", ntohs(packet.kv_pair_number), ntohs(packet.kv_pairs_total));

				proxy_string_modifier(packet.key);
				proxy_string_modifier(packet.value);

				if (tcp_send(client_connection_fd, &packet, sizeof(kv_packet)) == -1) {
					perror("Error: Failed to forward packet!");
					continue; // TODO: Is this needed?
				}

				printf("Forwarded packet from server to client\n");

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

