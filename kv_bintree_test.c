#include <kv_bintree.h>
#include <stdio.h>

int main(int argc, char *argv) {
	kv_binarytree *tree;
	char *temp;

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

return 0;
}