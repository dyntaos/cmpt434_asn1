#include <kv_bintree.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


kv_binarytree *alloc_kv_bintree(void) {
	kv_binarytree *kv;

	kv = (kv_binarytree*) malloc(sizeof(kv_binarytree));

	if (kv == NULL) {
		return NULL;
	}

	kv->root = NULL;
	kv->iterator = NULL;
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
		kv->iterator = NULL;
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




boolean add_kv_bintree_node(kv_binarytree_node *node, kv_binarytree_node *parent, kv_binarytree_node **parent_node, char *key, char *value) {
	int cmp;

	if (node == NULL) {
		node = alloc_node_kv_bintree();
		if (node == NULL) return FALSE;
		node->key = key;
		node->value = value;
		node->parent = parent;
		if (parent_node != NULL) {
			*parent_node = node;
		}
		return TRUE;
	}

	// Not using strncmp(), since the length bounds cannot be known
	// It's up to the user to ensure the keys are null-terminated
	cmp = strcmp(key, node->key);

	if (cmp == 0) return FALSE;

	if (cmp < 0) {
		return add_kv_bintree_node(node->left, node,  &(node->left), key, value);
	} else {
		return add_kv_bintree_node(node->right, node, &(node->right), key, value);
	}
}

boolean add_kv_bintree(kv_binarytree *kv, char *key, void *value) {
	boolean result;

	if (kv == NULL) return FALSE;

	if (kv->root == NULL) {
		kv->root = alloc_node_kv_bintree();
		if (kv->root == NULL) return FALSE;

		kv->root->key = key;
		kv->root->value = value;
		kv->size++;
		return TRUE;
	} else {
		result = add_kv_bintree_node(kv->root, NULL, NULL, key, value);
	}

	if (result) {
		kv->size++;
	}
	return result;
}




boolean has_kv_bintree_node(const kv_binarytree_node *node, const char *key) {
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

boolean has_kv_bintree(const kv_binarytree *kv, const char *key) {
	if (kv == NULL) return FALSE;
	return has_kv_bintree_node(kv->root, key);
}




boolean get_kv_bintree_node(const kv_binarytree_node *node, const char *key, void **value) {
	int cmp;

	if (node == NULL) return FALSE;

	// Not using strncmp(), since the length bounds cannot be known
	// It's up to the user to ensure the keys are null-terminated
	cmp = strcmp(key, node->key);

	if (cmp == 0) {
		*((char**) value) = (char*) node->value;
		return TRUE;
	}

	if (cmp < 0) {
		return get_kv_bintree_node(node->left, key, value);
	} else {
		return get_kv_bintree_node(node->right, key, value);
	}
}

boolean get_kv_bintree(const kv_binarytree *kv, const char *key, void **value) {
	if (kv == NULL) return FALSE;
	return get_kv_bintree_node(kv->root, key, value);
}




void delete_node_kv_bintree(kv_binarytree *kv, kv_binarytree_node *node) {
	kv_binarytree_node *subtree;
	boolean found_subtree = FALSE;

	if (node->left == NULL) {
		subtree = node->right;
		found_subtree = TRUE;
	} else if (node->right == NULL) {
		subtree = node->left;
		found_subtree = TRUE;
	}

	if (found_subtree) {
		if (node->parent == NULL) {
			kv->root = subtree;
			if (subtree != NULL) subtree->parent = NULL;
		} else if (node->parent->left == node) {
			node->parent->left = subtree;
			if (subtree != NULL) subtree->parent = node->parent;
			free(node);
		} else {
			node->parent->right = subtree;
			if (subtree != NULL) subtree->parent = node->parent;
			free(node);
		}
	} else {
		subtree = node->right;

		while (subtree->left != NULL) {
			subtree = subtree->left;
		}

		node->key = subtree->key;
		node->value = subtree->value;
		delete_node_kv_bintree(kv, subtree);
	}
}

void *remove_kv_bintree_node(kv_binarytree *kv, kv_binarytree_node *node, const char *key, void (*key_free)(void*)) {
	int cmp;
	void *value;

	if (node == NULL) return NULL;

	// Not using strncmp(), since the length bounds cannot be known
	// It's up to the user to ensure the keys are null-terminated
	cmp = strcmp(key, node->key);

	if (cmp == 0) {
		value = node->value;
		key_free(node->key);
		delete_node_kv_bintree(kv, node);
		kv->size--;
		if (kv->size == 0) {
			kv->root = NULL;
		}
		return value;
	}

	if (cmp < 0) {
		return remove_kv_bintree_node(kv, node->left, key, key_free);
	} else {
		return remove_kv_bintree_node(kv, node->right, key, key_free);
	}
}

void *remove_kv_bintree(kv_binarytree *kv, const char *key, void (*key_free)(void*)) {
	if (kv == NULL) return NULL;
	if (kv->root == NULL) return NULL;

	return remove_kv_bintree_node(kv, kv->root, key, key_free);
}




uint32_t size_kv_bintree(const kv_binarytree *kv) {
	if (kv == NULL) return 0;
	return kv->size;
}




void iterator_func_kv_bintree_node(kv_binarytree_node *node, void (iter_function)(char *k, void *v)) {
	if (node == NULL) return;

	iterator_func_kv_bintree_node(node->left, iter_function);
	iter_function(node->key, node->value);
	iterator_func_kv_bintree_node(node->right, iter_function);
}

boolean iterator_func_kv_bintree(kv_binarytree *kv, void (iter_function)(char *k, void *v)) {
	if (kv == NULL) return FALSE;
	if (kv->root == NULL) return FALSE;

	iterator_func_kv_bintree_node(kv->root, iter_function);
	return TRUE;
}




void cursor_deep_left_kv_bintree(kv_binarytree *kv) {
	if (kv == NULL) return;
	if (kv->root == NULL) return;
	if (kv->iterator == NULL) return;
	while (kv->iterator->left != NULL) {
		kv->iterator = kv->iterator->left;
	}
}

boolean cursor_init_kv_bintree(kv_binarytree *kv) {
	if (kv == NULL) return FALSE;
	if (kv->root == NULL) {
		// TODO
		kv->iterator = NULL;
	}

	kv->iterator = kv->root;
	cursor_deep_left_kv_bintree(kv);
	return TRUE;
}

boolean cursor_next_kv_bintree(kv_binarytree *kv) {
	kv_binarytree_node *node;

	if (kv == NULL) return FALSE;
	if (kv->root == NULL) return FALSE;
	if (kv->iterator == NULL) return FALSE;

	if (kv->iterator->right != NULL) {
		kv->iterator = kv->iterator->right;
		cursor_deep_left_kv_bintree(kv);
		return TRUE;
	}

	do {
		node = kv->iterator;

		if (kv->iterator->parent == NULL) {
			return FALSE;
		} else {
			kv->iterator = kv->iterator->parent;
		}
	} while (node != kv->iterator->left);
	return TRUE;
}

boolean cursor_get_kv_bintree(const kv_binarytree *kv, char **key, void **value) {
	if (kv == NULL) return FALSE;
	if (kv->iterator == NULL) return FALSE;

	*key = kv->iterator->key;
	*value = kv->iterator->value;
	return TRUE;
}




void print_in_order_kv_bintree_node(const kv_binarytree_node *node) {
	if (node == NULL) {
		return;
	}
	print_in_order_kv_bintree_node(node->left);
	printf("=========================\nAddress: %p"
		"\nkey: '%s'\t\tvalue: '%s'\nleft: %p\t\t"
		"right: %p\t\tparent: %p\n",
		(void*) node,
		node->key,
		(char*) node->value,
		(void*) node->left,
		(void*) node->right,
		(void*) node->parent
	);
	print_in_order_kv_bintree_node(node->right);
}

void print_in_order_kv_bintree(const kv_binarytree *kv) {
	if (kv == NULL) return;
	printf("=========================\n\n");
	printf("Root: %p\n", (void*) kv->root);

	print_in_order_kv_bintree_node(kv->root);

	printf("=========================\n\n");
}