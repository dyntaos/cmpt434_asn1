/**********************************
 **           CMPT 434           **
 **  University of Saskatchewan  **
 **         Assignment 1         **
 **----------------------------- **
 **          Kale Yuzik          **
 **     kay851@mail.usask.ca     **
 **      kay851    11071571      **
 **********************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include <kv_network.h>
#include <kv_packet.h>
#include <kv_bintree.h>



void validate_cli_args(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s PortNumber\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (strlen(argv[1]) > 5) {
		fprintf(stderr, "Invalid port number\n");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < strlen(argv[1]); i++) {
		if (!isdigit(argv[1][i])) {
			fprintf(stderr, "The port number provided must be numeric\n");
			exit(EXIT_FAILURE);
		}
	}

	if (strtoul(argv[1], NULL, 10) > 65535) {
		fprintf(stderr, "Port number must be between 0 to 65535\n");
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char *argv[]) {
	int sock_fd, new_fd;
	struct sockaddr_storage client_addr;
	kv_binarytree *tree;
	char *k, *v;
	size_t malloc_len, string_len;
	int recv_bytes;
	kv_packet packet;
	int quit_flag = 0;
	socklen_t addrlen;

#ifdef _UDP
	struct addrinfo *p;
#endif

	(void) argc;
	(void) argv;
	(void) addrlen;


	validate_cli_args(argc, argv);

	tree = alloc_kv_bintree();

	sock_fd = SOCKET_SERVER_INIT(argv[1], &p);
	if (sock_fd <= 0) {
		fprintf(stderr, "Failed to open socket to listen for client connections!\n");
		exit(EXIT_FAILURE);
	}

	printf("Server: Waiting for connections...\n");

	for (;;) {

		quit_flag = 0;
		new_fd = SOCKET_ACCEPT(sock_fd);
		if (new_fd < 0) continue;

#ifdef _TCP
		// Don't print this message if using UDP
		printf("Server: Got connection\n");
#endif // _TCP

		for (;;) {

			addrlen = sizeof(client_addr);

			if ((recv_bytes = SOCKET_RECEIVE(new_fd, &packet, sizeof(kv_packet), (struct sockaddr*) &client_addr, &addrlen)) == -1) {
				perror("Error: Failed to receive command from client/proxy");
				exit(EXIT_FAILURE);
			}

			printf("Received packet...\n");

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
						// Failed to add to binary tree
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

					if (SOCKET_SEND(new_fd, &packet, sizeof(kv_packet), (struct sockaddr*) &client_addr, addrlen) == -1) {
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

					if (SOCKET_SEND(new_fd, &packet, sizeof(kv_packet),  (struct sockaddr*) &client_addr, addrlen) == -1) {
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

						if (SOCKET_SEND(new_fd, &packet, sizeof(kv_packet), (struct sockaddr*) &client_addr, addrlen) == -1) {
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

							if (SOCKET_SEND(new_fd, &packet, sizeof(kv_packet), (struct sockaddr*) &client_addr, addrlen) == -1) {
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

					if (SOCKET_SEND(new_fd, &packet, sizeof(kv_packet), (struct sockaddr*) &client_addr, addrlen) == -1) {
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

