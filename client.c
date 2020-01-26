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
#include <string.h>
#include <ctype.h>

#include <kv_network.h>
#include <kv_packet.h>
#include <read_command.h>



void validate_cli_args(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage: %s ServerHostname ServerPort\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (strlen(argv[1]) < 2) {
		fprintf(stderr, "Provide a valid server hostname or IP address\n");
		exit(EXIT_FAILURE);
	}

	if (strlen(argv[2]) > 5) {
		fprintf(stderr, "Invalid server port number\n");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < strlen(argv[2]); i++) {
		if (!isdigit(argv[2][i])) {
			fprintf(stderr, "The server port number provided must be numeric\n");
			exit(EXIT_FAILURE);
		}
	}

	if (strtoul(argv[2], NULL, 10) > 65535) {
		fprintf(stderr, "Server port number must be between 0 to 65535\n");
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char *argv[]) {
	char *t1 = 0, *t2 = 0;
	kv_message_command cmd;
	struct addrinfo *p;
	int sockfd;


	(void) argc;
	(void) argv;
	(void) p;


	validate_cli_args(argc, argv);

	sockfd = SOCKET_CLIENT_INIT(argv[1], argv[2], &p);
	if (sockfd <= 0) {
		fprintf(stderr, "Failed to open connection to server!\n");
		exit(EXIT_FAILURE);
	}

	printf("Connected to server %s\n", argv[1]);


	for (;;) {
		int arg_count = 0;
		ssize_t recv_bytes;
		kv_packet packet;

		cmd = read_command(&t1, &t2);
		if (t1 != NULL) arg_count++;
		if (t2 != NULL) arg_count++;

		if (cmd == add && arg_count == 2) {

			if (!write_packet(&packet, cmd, REQUEST, 1, 1, t1, t2)) {
				// TODO: Failed to create packet (key and/or value too long!)
				fprintf(stderr, "Error: Invalid arguments to add!\n");
				continue;
			}

			if (SOCKET_SEND(sockfd, &packet, sizeof(kv_packet), p->ai_addr, p->ai_addrlen) == -1) {
				perror("Error: Failed to send add command!");
				continue; // TODO: Is this needed?
			}

			if ((recv_bytes = SOCKET_RECEIVE(sockfd, &packet, sizeof(kv_packet), NULL, NULL)) == -1) {
				perror("recv"); // TODO
				exit(EXIT_FAILURE);
			}

			if (recv_bytes == 0) {
				printf("Connection closed...\n");
				exit(EXIT_FAILURE);
			}

			if (ntohs(packet.kv_pairs_total) == 0) {  // Using ntohs() is redundant as bin-endian 0 == little-endian 0
				printf("Failed to add \"%s\":\"%s\"\n\n", t1, t2);
			} else {
				printf("Added \"%s\":\"%s\"\n\n", t1, t2);
			}

		} else if (cmd == get_value && arg_count == 1) {

			if (!write_packet(&packet, cmd, REQUEST, 1, 1, t1, t2)) {
				// TODO: Failed to create packet (key and/or value too long!)
				fprintf(stderr, "Make get_value packet\n");
				continue;
			}

			if (SOCKET_SEND(sockfd, &packet, sizeof(kv_packet), p->ai_addr, p->ai_addrlen) == -1) {
				perror("send getvalue");
				continue; // TODO: Is this needed?
			}

			if ((recv_bytes = SOCKET_RECEIVE(sockfd, &packet, sizeof(kv_packet), NULL, NULL)) == -1) {
				perror("recv"); // TODO
				exit(EXIT_FAILURE);
			}

			if (recv_bytes == 0) {
				printf("Connection closed...\n");
				exit(EXIT_FAILURE);
			}

			if (ntohs(packet.kv_pairs_total) == 0) {
				printf("Key \"%s\" does not exist\n\n", t1);
			} else {
				printf("\"%s\":\"%s\"\n\n", packet.key, packet.value);
			}

		} else if (cmd == get_all && arg_count == 0) {

			if (!write_packet(&packet, cmd, REQUEST, 0, 0, NULL, NULL)) {
				// TODO: Failed to create packet (key and/or value too long!)
				fprintf(stderr, "Make getall packet\n");
				continue;
			}

			if (SOCKET_SEND(sockfd, &packet, sizeof(kv_packet), p->ai_addr, p->ai_addrlen) == -1) {
				perror("send");
				continue; // TODO: Is this needed
			}

			do {
				if ((recv_bytes = SOCKET_RECEIVE(sockfd, &packet, sizeof(kv_packet), NULL, NULL)) == -1) {
					perror("recv"); // TODO
					exit(EXIT_FAILURE);
				}

				if (recv_bytes == 0) {
					printf("Connection closed...\n");
					exit(EXIT_FAILURE);
				}

				if (htons(packet.kv_pairs_total) == 0) {
					printf("Getall: 0 items\n\n");
				} else {
					if (ntohs(packet.kv_pair_number) == 1) {
						printf("Getall: %u items\n", htons(packet.kv_pairs_total));
					}
					printf("\"%s\":\"%s\"\n", packet.key, packet.value);
				}

			} while(ntohs(packet.kv_pair_number) != htons(packet.kv_pairs_total));
			printf("\n");


		} else if (cmd == remove_cmd && arg_count == 1) {

			if (!write_packet(&packet, cmd, REQUEST, 1, 1, t1, NULL)) {
				// TODO: Failed to create packet (key and/or value too long!)
				fprintf(stderr, "Make remove packet\n");
				continue;
			}

			if (SOCKET_SEND(sockfd, &packet, sizeof(kv_packet), p->ai_addr, p->ai_addrlen) == -1) {
				perror("send remove");
				continue; // TODO: Is this needed?
			}

			if ((recv_bytes = SOCKET_RECEIVE(sockfd, &packet, sizeof(kv_packet), NULL, NULL)) == -1) {
				perror("recv"); // TODO
				exit(EXIT_FAILURE);
			}

			if (recv_bytes == 0) {
				printf("Connection closed...\n");
				exit(EXIT_FAILURE);
			}

			if (ntohs(packet.kv_pairs_total) == 0) {
				printf("Key \"%s\" does not exist\n\n", t1);
			} else {
				printf("Removed \"%s\":\"%s\"\n\n", packet.key, packet.value);
			}

		} else if (cmd == quit && arg_count == 0) {

			if (!write_packet(&packet, cmd, REQUEST, 0, 0, NULL, NULL)) {
				// TODO: Failed to create packet (key and/or value too long!)
				fprintf(stderr, "Make quit packet\n");
				continue;
			}

			if (SOCKET_SEND(sockfd, &packet, sizeof(kv_packet), p->ai_addr, p->ai_addrlen) == -1) {
				perror("send quit");
				continue; // TODO: Is this needed?
			}
			exit(0);

		} else {
			printf("Invalid command\n");
		}
	}

	free_command(&t1, &t2);

	close(sockfd);

	return 0;
}

