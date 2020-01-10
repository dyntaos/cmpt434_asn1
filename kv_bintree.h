#ifndef _KV_BINTREE_H
#define _KV_BINTREE_H

#include <stdint.h>

#define NULL	0


typedef struct kv_binarytree_node_t kv_binarytree_node;

struct kv_binarytree_node_t {
	char *key, *value;
	kv_binarytree_node *left, *right;
};


typedef struct kv_binarytree_t {
	kv_binarytree *root;
	uint32_t size;
} kv_binarytree;


kv_binarytree *alloc_kv_bintree(void);
void free_kv_bintree(kv_binarytree *kv, void (*free_value)(void*));
int add_kv_bintree(kv_binarytree *kv, char *k, void *v);
int has_kv_bintree(kv_binarytree *kv, char *k);
void *get_kv_bintree(kv_binarytree *kv, char *k);
void iter_init_kv_bintree(kv_binarytree *kv);
int iter_get_kv_bintree(kv_binarytree *kv, char *k, void *v);
int iter_next_kv_bintree(kv_binarytree *kv);
int remove_kv_bintree(kv_binarytree *kv, char *k, void *v);


#endif //_KV_BINTREE_H