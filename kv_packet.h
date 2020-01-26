/**********************************
 **           CMPT 434           **
 **  University of Saskatchewan  **
 **         Assignment 1         **
 **----------------------------- **
 **          Kale Yuzik          **
 **     kay851@mail.usask.ca     **
 **      kay851    11071571      **
 **********************************/

#ifndef _KV_PACKET_H
#define _KV_PACKET_H

#include <stdint.h>

#include <kv_bintree.h>


#define KV_MAX_STRING_LEN			40
#define KV_STRING_FIELD_LEN			(KV_MAX_STRING_LEN * 2) + 1


typedef enum kv_message_command_e {invalid, add, get_value, get_all, remove_cmd, quit} kv_message_command;
typedef enum kv_message_type_e {REQUEST, REPLY} kv_message_type;


typedef struct kv_packet_t {
	uint8_t  message_command;
	uint8_t message_type;
	uint16_t kv_pairs_total;
	uint16_t kv_pair_number;
	char key[KV_STRING_FIELD_LEN];
	char value[KV_STRING_FIELD_LEN];
} kv_packet;



boolean write_packet(
	kv_packet *packet,
	kv_message_command msg_command,
	kv_message_type msg_direction,
	uint16_t kv_pairs_total,
	uint16_t kv_pair_number,
	char *key,
	char *value
);

#endif //_KV_PACKET_H