#ifndef _KV_PACKET_H
#define _KV_PACKET_H

#include <stdint.h>
#include <read_command.h>


//typedef enum message_type_e {add, get_value, get_all, remove, quit} message_type;


typedef struct kv_packet_header_t {
	uint16_t checksum;
	uint8_t  message_type;
	uint16_t kv_pair_segments;
} kv_packet_header;


typedef struct kv_pair_segment_t {
	uint16_t key_bytes;
	uint32_t value_bytes;
} kv_pair_segment;


uint16_t checksum(void *data, uint32_t data_bytes);
int verify_checksum(void *packet, uint32_t packet_bytes);

#endif //_KV_PACKET_H