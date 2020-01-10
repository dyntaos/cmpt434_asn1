#include <kv_bintree.h>

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

void free_kv_bintree_node(kv_binarytree_node *n, void (*free_key)(void*), void (*free_value)(void*)) {

	if (n->left != NULL) {
		free_kv_bintree_node(n->left, free_key, free_value);
		free(n->left);
		n->left = NULL;
	}

	if (n->right != NULL) {
		free_kv_bintree_node(n->right, free_key, free_value);
		free(n->right);
		n->right = NULL;
	}

	free_key(n->key);
	free_value(n->value);
	n->key = NULL;
	n->value = NULL;
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

int add_kv_bintree(kv_binarytree *kv, char *k, void *v) {

}

int has_kv_bintree(kv_binarytree *kv, char *k) {

}

void *get_kv_bintree(kv_binarytree *kv, char *k) {

}

void iter_init_kv_bintree(kv_binarytree *kv) {

}

int iter_get_kv_bintree(kv_binarytree *kv, char *k, void *v) {

}

int iter_next_kv_bintree(kv_binarytree *kv) {

}

int remove_kv_bintree(kv_binarytree *kv, char *k, void *v) {

}
