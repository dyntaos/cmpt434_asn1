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

#include <kv_packet.h>
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
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	kv_binarytree *tree;

	int32_t recv_bytes;
	kv_packet_header recv_header;
	void *recv_payload = NULL;


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

	if (p == NULL)  {
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

	printf("server: waiting for connections...\n");

	for (;;) {
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr*) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr*) &their_addr), s, sizeof(s));
		printf("server: got connection from %s\n", s);

		for (;;) {

			if ((recv_bytes = recv(new_fd, &recv_header, sizeof(kv_packet_header), 0)) == -1) {
				perror("recv");
				exit(1);
			}

			printf("checksum: %u  pair segments: %u  message type: %u  payload bytes: %u\n", recv_header.checksum, recv_header.kv_pair_segments, recv_header.message_type, recv_header.payload_bytes);

			recv_payload = malloc(recv_header.payload_bytes);

			if ((recv_bytes = recv(new_fd, recv_payload, recv_header.payload_bytes, 0)) == -1) {
				perror("recv2");
				exit(1);
			}

			printf("Got payload\n");


			char *k, *v;
			k = (char*) malloc(((kv_pair_segment*)recv_payload)->key_bytes + 1);
			v = (char*) malloc(((kv_pair_segment*)recv_payload)->value_bytes + 1);

			memcpy(
				k,
				&(((char*) recv_payload)[sizeof(kv_pair_segment)]),
				((kv_pair_segment*)recv_payload)->key_bytes
			);
			k[((kv_pair_segment*)recv_payload)->key_bytes] = 0;

			memcpy(
				v,
				&(((char*) recv_payload)[sizeof(kv_pair_segment) + ((kv_pair_segment*)recv_payload)->key_bytes]),
				((kv_pair_segment*)recv_payload)->value_bytes
			);
			v[((kv_pair_segment*)recv_payload)->value_bytes] = 0;

			add_kv_bintree(tree, k, v);

			print_in_order_kv_bintree(tree);
		}

		close(new_fd);
		/*
		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			if (send(new_fd, "Hello, world!", 13, 0) == -1)
				perror("send");
			close(new_fd);
			exit(0);
		}
		*/


	}

	return 0;
}

