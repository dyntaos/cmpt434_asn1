#include <kv_bintree.h>
#include <string.h>
#include <stdlib.h>


kv_binarytree *alloc_kv_bintree(void) {
	kv_binarytree *kv;

	kv = (kv_binarytree*) malloc(sizeof(kv_binarytree));

	if (kv == NULL) {
		return NULL;
	}

	kv->root = NULL;
	kv->size = 0;

	return kv;
}

void free_kv_bintree_node(kv_binarytree_node *node, void (*free_key)(void*), void (*free_value)(void*)) {

	if (node->left != NULL) {
		free_kv_bintree_node(node->left, free_key, free_value);
		free(node->left);
		node->left = NULL;
	}

	if (node->right != NULL) {
		free_kv_bintree_node(node->right, free_key, free_value);
		free(node->right);
		node->right = NULL;
	}

	free_key(node->key);
	free_value(node->value);
	node->parent = NULL;
	node->key = NULL;
	node->value = NULL;
}

void free_kv_bintree(kv_binarytree *kv, void (*free_key)(void*), void (*free_value)(void*)) {
	if (kv == NULL) return;
	if (kv->root != NULL) {
		free_kv_bintree_node(kv->root, free_key, free_value);
		free(kv->root);
		kv->root = NULL;
	}
	free(kv);
}

kv_binarytree_node *alloc_node_kv_bintree(void) {
	kv_binarytree_node *n;

	n = (kv_binarytree_node*) malloc(sizeof(kv_binarytree_node));

	if (n == NULL) return NULL;

	n->key = NULL;
	n->value = NULL;
	n->left = NULL;
	n->right = NULL;
	n->parent = NULL;
	return n;
}

boolean add_kv_bintree_node(kv_binarytree_node *node, kv_binarytree_node *parent, char *key, char *value) {
	int cmp;

	if (node == NULL) {
		node = alloc_node_kv_bintree();
		if (node == NULL) return FALSE;

		node->key = key;
		node->value = value;
		node->parent = p;
		return TRUE;
	}

	// Not using strncmp(), since the length bounds cannot be known
	// It's up to the user to ensure the keys are null-terminated
	cmp = strcmp(key, node->key);

	if (cmp == 0) return FALSE;

	if (cmp < 0) {
		return add_kv_bintree_node(node->left, node, key, value);
	} else {
		return add_kv_bintree_node(node->right, node, key, value);
	}
}

boolean add_kv_bintree(kv_binarytree *kv, char *key, void *value) {
	boolean result;

	if (kv == NULL) return FALSE;

	result = add_kv_bintree_node(kv, NULL, key, value);

	if (result) {
		kv->size++;
	}
	return result;
}

boolean has_kv_bintree_node(kv_binarytree_node *node, char *key) {
	int cmp;

	if (node == NULL) return FALSE;

	// Not using strncmp(), since the length bounds cannot be known
	// It's up to the user to ensure the keys are null-terminated
	cmp = strcmp(key, node->key);

	if (cmp == 0) return TRUE;

	if (cmp < 0) {
		return has_kv_bintree_node(node->left, key);
	} else {
		return has_kv_bintree_node(node->right, key);
	}
}

boolean has_kv_bintree(kv_binarytree *kv, char *key) {
	if (kv == NULL) return FALSE;
	return has_kv_bintree_node(kv->root, key);
}

boolean *get_kv_bintree_node(kv_binarytree_node *node, char *key, void **value) {
	int cmp;

	if (node == NULL) return FALSE;

	// Not using strncmp(), since the length bounds cannot be known
	// It's up to the user to ensure the keys are null-terminated
	cmp = strcmp(key, node->key);

	if (cmp == 0) {
		*value = node->value;
		return TRUE;
	}

	if (cmp < 0) {
		return get_kv_bintree_node(node->left, key, value);
	} else {
		return get_kv_bintree_node(node->right, key, value);
	}
}

boolean *get_kv_bintree(kv_binarytree *kv, char *key, void **value) {
	if (kv == NULL) return FALSE;
	return get_kv_bintree_node(kv->root, key, value);
}

void iter_init_kv_bintree(kv_binarytree *kv) {

}

boolean iter_get_kv_bintree(kv_binarytree *kv, char *key, void *value) {

}

boolean iter_next_kv_bintree(kv_binarytree *kv) {

}

boolean remove_kv_bintree(kv_binarytree *kv, char *key, void *value) {

}
