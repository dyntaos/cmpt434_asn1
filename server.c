#include <server.h>
#include <string.h>
#include <stdlib.h>

boolean server_add(kv_binarytree *tree, void *recv_payload) {
	char *k, *v;
	k = (char*) malloc(((kv_pair_segment*) recv_payload)->key_bytes + 1);
	v = (char*) malloc(((kv_pair_segment*) recv_payload)->value_bytes + 1);

	memcpy(
		k,
		&(((char*) recv_payload)[sizeof(kv_pair_segment)]),
		((kv_pair_segment*) recv_payload)->key_bytes
	);
	k[((kv_pair_segment*) recv_payload)->key_bytes] = 0;

	memcpy(
		v,
		&(((char*) recv_payload)[sizeof(kv_pair_segment) + ((kv_pair_segment*) recv_payload)->key_bytes]),
		((kv_pair_segment*) recv_payload)->value_bytes
	);
	v[((kv_pair_segment*) recv_payload)->value_bytes] = 0;

	return add_kv_bintree(tree, k, v);
}