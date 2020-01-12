#include <kv_packet.h>
#include <string.h>
#include <stdlib.h>

uint16_t checksum(void *packet_data, uint32_t packet_bytes) {
	uint16_t *packet = (uint16_t*) (((char*) packet_data) + 2);
	uint32_t half_words = (packet_bytes - 2) / 2;
	uint8_t odd_bytes = packet_bytes % 2;
	uint16_t chk = 0;

	for (uint32_t i = 2; i < half_words; i++) {
		chk += ~(packet[i]);
	}
	if (odd_bytes) {
		chk += ~( ((char*) packet)[packet_bytes - 1]);
	}
	return chk;
}

int verify_checksum(void *packet, uint32_t packet_bytes) {
	uint16_t packet_checksum = *((uint16_t*) packet);
	uint16_t calculated_sum = 0;
	uint16_t *p = (uint16_t*) packet;
	uint8_t odd_bytes = packet_bytes % 2;

	for (uint32_t i = 2; i < packet_bytes / 2; i++) {
		calculated_sum += p[i];
	}
	if (odd_bytes) {
		calculated_sum += ((char*) packet)[packet_bytes - 1];
	}
	return calculated_sum + packet_checksum == 0xFFFF;
}

void *create_client_packet(command cmd, uint32_t *packet_bytes, char *key, char *value) {
	uint32_t key_len, value_len;
	void *packet = 0;

	key_len = strlen(key);
	value_len = strlen(value);

	if (cmd == add) {
		kv_packet_header *header;
		kv_pair_segment *kv_segment;
		*packet_bytes = sizeof(kv_packet_header) + sizeof(kv_pair_segment) + key_len + value_len;
		packet = malloc(*packet_bytes);
		header = packet;
		kv_segment = (kv_pair_segment*) (((char*) packet) + sizeof(kv_packet_header));

		header->message_type = (uint8_t) cmd;
		header->kv_pair_segments = 1;

		kv_segment->key_bytes = key_len;
		kv_segment->value_bytes = value_len;
		memcpy(kv_segment + sizeof(kv_pair_segment), key, key_len);
		memcpy(kv_segment + sizeof(kv_pair_segment) + key_len, value, value_len);

	} else if (cmd == get_value) {

	} else if (cmd == get_all) {

	} else if (cmd == remove_cmd) {

	} else if (cmd == quit) {

	} else {
		return NULL;
	}
	return NULL; // TODO
}