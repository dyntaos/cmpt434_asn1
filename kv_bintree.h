#ifndef _KV_BINTREE_H
#define _KV_BINTREE_H

#include <stdint.h>

#ifndef NULL
#define NULL	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

enum kv_bintree_iterator_state {uninitialized, on_left, on_self, on_right};

typedef struct kv_binarytree_node_t kv_binarytree_node;

struct kv_binarytree_node_t {
	char *key;
	void *value;
	kv_binarytree_node *left, *right, *parent;
};


typedef struct kv_binarytree_t {
	kv_binarytree_node *root, *iterator, *prev_iterator;
	enum kv_bintree_iterator_state iterator_state;
	uint32_t size;
} kv_binarytree;


typedef int boolean;


kv_binarytree *alloc_kv_bintree(void);
void free_kv_bintree(kv_binarytree *kv, void (*free_key)(void*), void (*free_value)(void*));
boolean add_kv_bintree(kv_binarytree *kv, char *key, void *value);
boolean has_kv_bintree(kv_binarytree *kv, char *key);
boolean get_kv_bintree(kv_binarytree *kv, char *key, void **value);
uint32_t size_kv_bintree(kv_binarytree *kv);
boolean iter_init_kv_bintree(kv_binarytree *kv);
boolean iter_get_kv_bintree(kv_binarytree *kv, char *key, void **value);
boolean iter_next_kv_bintree(kv_binarytree *kv);
boolean remove_kv_bintree(kv_binarytree *kv, char *key, void *value);
void print_in_order_kv_bintree(kv_binarytree *kv);


#endif //_KV_BINTREE_H