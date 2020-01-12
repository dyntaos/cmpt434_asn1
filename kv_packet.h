#ifndef _KV_PACKET_H
#define _KV_PACKET_H

#include <stdint.h>


typedef enum message_type_e {add, get_value, get_all, remove, quit} message_type;


typedef struct kv_packet_header_t {
	uint8_t  message_type;
	uint16_t kv_pair_segments;
	uint16_t checksum;
} kv_packet_header;


typedef struct kv_pair_segment_t {
	uint16_t key_bytes;
	uint32_t value_bytes;
} kv_pair_segment;


#endif //_KV_PACKET_H