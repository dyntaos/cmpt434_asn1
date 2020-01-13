#ifndef _KV_SERVER_PACKET_H
#define _KV_SERVER_PACKET_H

#include <kv_packet.h>


void *create_server_packet(kv_binarytree *tree, const command cmd, uint32_t *packet_bytes, const char *key);

void *create_server_getall_packet(kv_binarytree *tree, uint32_t *packet_bytes);

#endif //_KV_SERVER_PACKET_H