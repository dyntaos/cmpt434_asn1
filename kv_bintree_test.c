#include <kv_bintree.h>
#include <stdio.h>


void tree_iterator(char *k, void *v) {
	printf("\t\"%s\": \"%s\"\n", k, (char*) v);
}


int main(int argc, char *argv[]) {
	kv_binarytree *tree;
	char *temp, *k;
	void *v;

	(void) argc;
	(void) argv;

	tree = alloc_kv_bintree();

	if (tree == NULL) perror("Failed to create binary tree");

	printf("Size of tree: %u\n", size_kv_bintree(tree));

	printf("Inserting \"key1\":\"value1\"\n");
	add_kv_bintree(tree, "key1", "value1");

	printf("Size of tree: %u\n", size_kv_bintree(tree));

	if (has_kv_bintree(tree, "key1")) {
		printf("Tree contains key \"key1\"\n");
	} else {
		printf("Tree does not contain key \"key1\"\n");
	}

	get_kv_bintree(tree, "key1", (void**) &temp);
	printf("Get \"key1\" = %s\n\n", temp);

	print_in_order_kv_bintree(tree);

	printf("Inserting \"a key 2\":\"a value  2\"\n");
	add_kv_bintree(tree, "a key 2", "a value  2");

	printf("Size of tree: %u\n", size_kv_bintree(tree));

	if (has_kv_bintree(tree, "a key 2")) {
		printf("Tree contains key \"a key 2\"\n");
	} else {
		printf("Tree does not contain key \"a key 2\"\n");
	}

	get_kv_bintree(tree, "a key 2", (void**) &temp);
	printf("Get \"a key 2\" = %s\n\n", temp);

	printf("Inserting \"zzzz\":\"turtles\"\n");
	add_kv_bintree(tree, "zzzz", "turtles");

	printf("Inserting \"llllllllllll\":\"$$\"\n");
	add_kv_bintree(tree, "llllllllllll", "$$");

	printf("Inserting \"bcd\":\"test\"\n");
	add_kv_bintree(tree, "bcd", "test");

	printf("Inserting \"ttt\":\"@\"\n");
	add_kv_bintree(tree, "ttt", "@");

	print_in_order_kv_bintree(tree);

	printf("Size of tree: %u\n", size_kv_bintree(tree));

	get_kv_bintree(tree, "llllllllllll", (void**) &temp);
	printf("Get \"llllllllllll\" = %s\n\n", temp);

	get_kv_bintree(tree, "bcd", (void**) &temp);
	printf("Get \"bcd\" = %s\n\n", temp);

	get_kv_bintree(tree, "ttt", (void**) &temp);
	printf("Get \"ttt\" = %s\n\n", temp);

	iterator_func_kv_bintree(tree, tree_iterator);

	printf("\n\n");

	cursor_init_kv_bintree(tree);
	do {
		cursor_get_kv_bintree(tree, &k, &v);
		printf("Cursor key: \"%s\"  value: \"%s\"\n", k, (char*) v);
	} while (cursor_next_kv_bintree(tree));

	if (remove_kv_bintree(tree, "fake") == NULL) {
		printf("Failed to delete key \"fake\", as expected\n\n");
	}

	printf("Delete key \"key1\"!\n\n");
	if (remove_kv_bintree(tree, "key1") == NULL) {
		printf("Failed to delete key \"key1\"...\n\n");
	}

	printf("Delete key \"a key 2\"...\n\n");
	if (remove_kv_bintree(tree, "a key 2") == NULL) {
		printf("Failed to delete key \"a key 2\"!\n\n");
	}

	printf("Delete key \"llllllllllll\"...\n\n");
	if (remove_kv_bintree(tree, "llllllllllll") == NULL) {
		printf("Failed to delete key \"llllllllllll\"!\n\n");
	}

	printf("Delete key \"ttt\"...\n\n");
	if (remove_kv_bintree(tree, "ttt") == NULL) {
		printf("Failed to delete key \"ttt\"!\n\n");
	}


	print_in_order_kv_bintree(tree);


return 0;
}