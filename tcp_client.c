/*
** client.c -- a stream socket client demo
** https://beej.us/guide/bgnet/html/
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <kv_packet.h>
#include <read_command.h>

#define PORT "3490" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char *argv[]) {
	char *t1 = 0, *t2 = 0;
	kv_message_command cmd;

	int sockfd;
	//int numbytes;
	//char buf[MAXDATASIZE];
	struct addrinfo hints;
	struct addrinfo *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	(void) argc;
	(void) argv;

	//if (argc != 2) {
	//    fprintf(stderr,"usage: client hostname\n");
	//    exit(1);
	//}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;


	if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure



	for (;;) {
		int arg_count = 0;
		ssize_t recv_bytes;
		kv_packet packet;

		cmd = read_command(&t1, &t2);
		if (t1 != NULL) arg_count++;
		if (t2 != NULL) arg_count++;
		//printf("cmd: %u  t1: \"%s\"  t2: \"%s\"\n", cmd, t1, t2);

		if (cmd == add && arg_count == 2) {

			if (!write_packet(&packet, cmd, REQUEST, 1, 1, t1, t2)) {
				// TODO: Failed to create packet (key and/or value too long!)
				perror("Error: Invalid arguments to add!");
				continue;
			}

			if (send(sockfd, &packet, sizeof(kv_packet), 0) == -1) {
				perror("Error: Failed to send add command!");
				continue; // TODO: Is this needed?
			}

			if ((recv_bytes = recv(sockfd, &packet, sizeof(kv_packet), 0)) == -1) {
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
				perror("Make get_value packet");
				continue;
			}

			if (send(sockfd, &packet, sizeof(kv_packet), 0) == -1) {
				perror("send getvalue");
				continue; // TODO: Is this needed?
			}

			if ((recv_bytes = recv(sockfd, &packet, sizeof(kv_packet), 0)) == -1) {
				perror("recv"); // TODO
				exit(EXIT_FAILURE);
			}

			if (recv_bytes == 0) {
				printf("Connection closed...\n");
				exit(EXIT_FAILURE);
			}

			if (ntohs(packet.kv_pairs_total) == 0) {  // Using ntohs() is redundant as bin-endian 0 == little-endian 0
				printf("Key \"%s\" does not exist\n\n", t1);
			} else {
				printf("\"%s\":\"%s\"\n\n", packet.key, packet.value);
			}

		} else if (cmd == get_all && arg_count == 0) {

			if (!write_packet(&packet, cmd, REQUEST, 0, 0, NULL, NULL)) {
				// TODO: Failed to create packet (key and/or value too long!)
				perror("Make getall packet");
				continue;
			}

			if (send(sockfd, &packet, sizeof(kv_packet), 0) == -1) {
				perror("send");
				continue; // TODO: Is this needed
			}

			do {
				if ((recv_bytes = recv(sockfd, &packet, sizeof(kv_packet), 0)) == -1) {
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
				perror("Make remove packet");
				continue;
			}

			if (send(sockfd, &packet, sizeof(kv_packet), 0) == -1) {
				perror("send remove");
				continue; // TODO: Is this needed?
			}

			if ((recv_bytes = recv(sockfd, &packet, sizeof(kv_packet), 0)) == -1) {
				perror("recv"); // TODO
				exit(EXIT_FAILURE);
			}

			if (recv_bytes == 0) {
				printf("Connection closed...\n");
				exit(EXIT_FAILURE);
			}

			if (ntohs(packet.kv_pairs_total) == 0) {  // Using ntohs() is redundant as bin-endian 0 == little-endian 0
				printf("Key \"%s\" does not exist\n\n", t1);
			} else {
				printf("Removed \"%s\":\"%s\"\n\n", packet.key, packet.value);
			}

		} else if (cmd == quit && arg_count == 0) {

			if (!write_packet(&packet, cmd, REQUEST, 0, 0, NULL, NULL)) {
				// TODO: Failed to create packet (key and/or value too long!)
				perror("Make quit packet");
				continue;
			}

			if (send(sockfd, &packet, sizeof(kv_packet), 0) == -1) {
				perror("send quit");
				continue; // TODO: Is this needed?
			}
			exit(0);

		} else {
			printf("Invalid command\n");
		}
	}

	free_command(&t1, &t2);

	/*
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	buf[numbytes] = '\0';
	*/

	close(sockfd);

	return 0;
}

