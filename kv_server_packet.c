#include <string.h>
#include <stdlib.h>

#include <kv_server_packet.h>



void *create_server_packet(kv_binarytree *tree, const command cmd, uint32_t *packet_bytes, const char *key) {
	void *value;

	switch (cmd) {
		case add:

			break;
		case get_value:
			if (get_kv_bintree(tree, key, &value)) {
				return create_kv_pair_packet(cmd, packet_bytes, key, value);
			}

		case get_all:
			return create_server_getall_packet(tree, packet_bytes);

		case remove_cmd:

			break;
		default:
			break;
	}
	return NULL;
}

void *create_server_getall_packet(kv_binarytree *tree, uint32_t *packet_bytes) {
	char *key;
	void *value;
	uint32_t key_len, value_len, pairs = 0, payload_bytes = 0;
	void *packet = 0;
	kv_packet_header *header;
	kv_pair_segment *kv_segment;

	cursor_init_kv_bintree(tree);

	do {
		cursor_get_kv_bintree(tree, &key, &value);
		payload_bytes += sizeof(kv_pair_segment) + strlen(key) + strlen(value);
		pairs++;
	} while(cursor_next_kv_bintree(tree));

	packet = malloc(*packet_bytes);
	header = packet;
	kv_segment = (kv_pair_segment*) (((char*) packet) + sizeof(kv_packet_header));

	header->payload_bytes = payload_bytes;
	*packet_bytes = sizeof(kv_packet_header) + header->payload_bytes;
	header->message_type = (uint8_t) get_all;
	header->kv_pair_segments = pairs;

	cursor_init_kv_bintree(tree);

	do {
		cursor_get_kv_bintree(tree, &key, &value);
		key_len = strlen(key);
		value_len = strlen(value);
		packet_write_kv_segment(kv_segment, key, key_len, value, value_len);

		kv_segment = (kv_pair_segment*) (((char*) kv_segment) + key_len + value_len);
	} while(cursor_next_kv_bintree(tree));

	return packet;
}
