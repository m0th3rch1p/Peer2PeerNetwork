#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binaryTree.h"

void insert_binary_tree(void* data, int dataLen, struct BinaryTree* tree);
void remove_binary_tree(void* data, struct BinaryTree* tree);
void* search_binary_tree(void* data, struct BinaryTree* tree);

struct BinaryTreeNode* binary_tree_node_constructor(void* data, int dataLen);
void binary_tree_node_deconstructor(struct BinaryTreeNode* tree_node);

struct BinaryTreeNode* prevInOrderTraversal(struct BinaryTreeNode* cursor);

struct BinaryTree* binary_tree_constructor(int (*cmp)(void* data1, void* data2)) {
	struct BinaryTree* tree = (struct BinaryTree*)calloc(1, sizeof(struct BinaryTree));
	if (!tree) {
		fprintf(stderr, "[-] error allocating memory for binary tree\n");
		return NULL;
	}
		
	tree->cmp = cmp;
	tree->insert = insert_binary_tree;
	tree->remove = remove_binary_tree;
	tree->search = search_binary_tree;

	return tree;
}

void destroy_nodes(struct BinaryTreeNode* treeNode) {
	if (treeNode->left) destroy_nodes(treeNode->left);
	if (treeNode->right) destroy_nodes(treeNode->right);

	binary_tree_node_deconstructor(treeNode);
}

void binary_tree_deconstructor(struct BinaryTree* tree) {
	destroy_nodes(tree->root);
	free(tree);
}

struct BinaryTreeNode* binary_tree_node_constructor(void* data, int dataLen) {
	struct BinaryTreeNode* tree_node = (struct BinaryTreeNode*)calloc(1, sizeof(struct BinaryTreeNode));
	if (!tree_node) {
		fprintf(stderr, "[-] error allocating memory for tree node\n");
		return NULL;
	}

	tree_node->node = node_constructor(data, dataLen);

	return tree_node;
}

void binary_tree_node_deconstructor(struct BinaryTreeNode* tree_node) {
	node_deconstructor(tree_node->node);
	free(tree_node);
}

struct BinaryTreeNode* iterate_tree(void* data, struct BinaryTreeNode* cursor, int* direction, int (*cmp)(void* data1, void* data2)) {
	/// <summary>
	/// data1 -> user data
	/// data -> struct data
	/// </summary>
	*direction = cmp(data, cursor->node->data);
	if (*direction < 0 && cursor->left) {
		*direction = -1;
		cursor = cursor->left;
	}
	else if (*direction > 0 && cursor->right) {
		*direction = 1;
		cursor = cursor->right;
	} else return cursor;

	iterate_tree(data, cursor, direction, cmp);
}

/// <summary>
/// Return previous most left node
/// </summary>
struct BinaryTreeNode* prevInOrderTraversal(struct BinaryTreeNode* cursor) {
	while (cursor->left->left) cursor = cursor->left;
	return cursor;
}

void insert_binary_tree(void* data, int dataLen, struct BinaryTree* tree) {
	struct BinaryTreeNode* node_to_insert = binary_tree_node_constructor(data, dataLen);
	if (!node_to_insert) return;


	if (!tree->root) {
		tree->root = node_to_insert;
		return;
	}

	int direction = -2;
	struct BinaryTreeNode* cursor = iterate_tree(data, tree->root, &direction, tree->cmp);
	if (direction == 0) {
		fprintf(stderr, "[-] item already exists\n");
		binary_tree_node_deconstructor(node_to_insert);
		return;
	}

	if (direction == -1) cursor->left = node_to_insert;
	else if (direction == 1) cursor->right = node_to_insert;
}

void remove_binary_tree(void* data, struct BinaryTree* tree) {
	int direction = -2;
	struct BinaryTreeNode* cursor = iterate_tree(data, tree->root, &direction, tree->cmp);
	if (!cursor) {
		fprintf(stderr, "[-] item not found in tree\n");
		return;
	}

	//Get left most node and copy data to node to remove and remove the node later
	struct BinaryTreeNode* prevNode = prevInOrderTraversal(cursor->right);
	
	copy_node_data(cursor->node, prevNode->left->node);
	binary_tree_node_deconstructor(prevNode->left);

	prevNode->left = NULL;
}

void* search_binary_tree(void* data, struct BinaryTree* tree) {
	int direction = -2;

	struct BinaryTreeNode* cursor = iterate_tree(data, tree->root, &direction, tree->cmp);
	return cursor->node->data;
}