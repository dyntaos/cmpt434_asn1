#include <kv_bintree.h>
#include <string.h>


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

kv_binarytree_node *alloc_node_kv_bintree(void) {
	kv_binarytree_node *n;

	n = (kv_binarytree_node*) malloc(sizeof(kv_binarytree_node));

	if (n == NULL) return NULL;

	n->key = NULL;
	n->value = NULL;
	n->left = NULL;
	n->right = NULL;
	return n;
}

int add_kv_bintree_node(kv_binarytree_node *n, char *k, char *v) {
	int cmp;

	if (n == NULL) {
		n = alloc_node_kv_bintree();
		if (n == NULL) return FALSE;

		n->key = k;
		n->value = v;
		return TRUE;
	}

	// Not using strncmp(), since the length bounds cannot be known
	// It's up to the user to ensure the keys are null-terminated
	cmp = strcmp(k, n->key);

	if (cmp == 0) {
		// Keys match!
		return FALSE;
	}

	if (cmp < 0) {
		return add_kv_bintree_node(n->left, k, v);
	} else {
		return add_kv_bintree_node(n->right, k, v);
	}
}

int add_kv_bintree(kv_binarytree *kv, char *k, void *v) {
	int result;

	if (kv == NULL) return;

	result = add_kv_bintree_node(kv, k, v);

	if (result) {
		kv->size++;
	}
	return result;
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
