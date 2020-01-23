#include <string.h>
#include <stdlib.h>

#include <kv_packet.h>



boolean write_packet(
	kv_packet *packet,
	kv_message_command msg_command,
	kv_message_type msg_direction,
	uint16_t kv_pairs_total,
	uint16_t kv_pair_number,
	char *key,
	char *value
) {
	size_t key_len, value_len;

	if (key != NULL) {
		key_len = strlen(key);
		if (key_len > KV_MAX_STRING_LEN) return FALSE;
	} else {
		key_len = 0;
	}

	if (value != NULL) {
		value_len = strlen(value);
		if (value_len > KV_MAX_STRING_LEN) return FALSE;
	} else {
		value_len = 0;
	}

	packet->message_command = msg_command;
	packet->message_type = msg_direction;
	packet->kv_pairs_total = kv_pairs_total;
	packet->kv_pair_number = kv_pair_number;


	//TODO REMOVE
	memset(packet->key, 0, KV_STRING_FIELD_LEN);
	memset(packet->value, 0, KV_STRING_FIELD_LEN);\


	if (key != NULL) {
		memcpy(packet->key, key, key_len);
	}
	// Write NULL to unused characters to prevent leaking of uninitialized memory
	//memset(packet->key + key_len, 0, KV_STRING_FIELD_LEN - key_len); // TODO: Check the length written here

	if (value != NULL) {
		memcpy(packet->value, value, value_len);
	}
	// Write NULL to unused characters to prevent leaking of uninitialized memory
	memset(packet->value + value_len, 0, KV_STRING_FIELD_LEN - value_len); // TODO: Check the length written here

	return TRUE;
}

