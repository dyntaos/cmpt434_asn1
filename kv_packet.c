#include <string.h>
#include <stdlib.h>

#include <kv_packet.h>


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

void *create_client_packet(const command cmd, uint32_t *packet_bytes, const char *key, const char *value) {
	uint32_t key_len; //, value_len;
	void *packet = 0;
	kv_packet_header *header;
	kv_pair_segment *kv_segment;

	if (key != NULL) {
		key_len = strlen(key);
	} else {
		key_len = 0;
	}
	//value_len = strlen(value);

	if (cmd == add) {
		return create_kv_pair_packet(cmd, packet_bytes, key, value);

	} else if (cmd == get_value || cmd == remove_cmd) {
		*packet_bytes = sizeof(kv_packet_header) + sizeof(kv_pair_segment) + key_len;
		packet = malloc(*packet_bytes);
		header = packet;
		kv_segment = (kv_pair_segment*) (((char*) packet) + sizeof(kv_packet_header));

		header->payload_bytes = sizeof(kv_pair_segment) + key_len;
		header->message_type = (uint8_t) cmd;
		header->kv_pair_segments = 1;

		packet_write_kv_segment(kv_segment, key, key_len, NULL, 0);

		//kv_segment->key_bytes = key_len;
		//kv_segment->value_bytes = 0;
		//memcpy(((char*) kv_segment) + sizeof(kv_pair_segment), key, key_len);

	} else if (cmd == get_all || cmd == quit) {
		*packet_bytes = sizeof(kv_packet_header);
		packet = malloc(*packet_bytes);
		header = packet;

		header->payload_bytes = 0;
		header->message_type = (uint8_t) cmd;
		header->kv_pair_segments = 0;

	} else {
		return NULL;
	}
	return packet;
}

void packet_write_kv_segment(kv_pair_segment *segment, const char *key, uint32_t key_len, const char *value, uint32_t value_len) {
	segment->key_bytes = key_len;
	segment->value_bytes = value_len;
	memcpy(((char*) segment) + sizeof(kv_pair_segment), key, key_len);
	memcpy(((char*) segment) + sizeof(kv_pair_segment) + key_len, value, value_len);
}

void *create_kv_pair_packet(const command cmd, uint32_t *packet_bytes, const char *key, const char *value) {
	uint32_t key_len = strlen(key);
	uint32_t value_len = strlen(value);
	void *packet = 0;
	kv_packet_header *header;
	kv_pair_segment *kv_segment;

	if (cmd != add && cmd != remove_cmd) return NULL;

	*packet_bytes = sizeof(kv_packet_header) + sizeof(kv_pair_segment) + key_len + value_len;
	packet = malloc(*packet_bytes);
	header = packet;
	kv_segment = (kv_pair_segment*) (((char*) packet) + sizeof(kv_packet_header));
	packet_write_kv_segment(kv_segment, key, key_len, value, value_len);

	header->payload_bytes = sizeof(kv_pair_segment) + key_len + value_len;
	header->message_type = (uint8_t) cmd;
	header->kv_pair_segments = 1;

	return packet;
}
