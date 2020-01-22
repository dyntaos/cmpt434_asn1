#ifndef _KV_PACKET_H
#define _KV_PACKET_H

#include <stdint.h>

#include <read_command.h>
#include <kv_bintree.h>

typedef enum message_direction_e {REQUEST, REPLY} message_direction;

typedef struct kv_packet_header_t {
	uint16_t checksum;
	//uint32_t payload_bytes;
	uint8_t  message_type;
	uint8_t message_direction;
	uint16_t kv_pair_segments;
} kv_packet_header;


typedef struct kv_pair_segment_t {
	uint16_t key_bytes;
	uint32_t value_bytes;
} kv_pair_segment;


uint16_t checksum(void *data, uint32_t data_bytes);
int verify_checksum(void *packet, uint32_t packet_bytes);

void *create_client_packet(const command cmd, uint32_t *packet_bytes, const char *key, const char *value);

void packet_write_kv_segment(kv_pair_segment *segment, const char *key, uint32_t key_len, const char *value, uint32_t value_len);
void *create_kv_pair_packet(const command cmd, uint32_t *packet_bytes, const char *key, const char *value);
void getKVfromSegment(void *recv_payload, void **k, void **v);

#endif //_KV_PACKET_H