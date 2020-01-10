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

void free_kv_bintree(kv_binarytree *kv, void (*free_value)(void*)) {

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
