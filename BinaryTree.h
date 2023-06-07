#ifndef BINARYTREE_H
#define BINARYTREE_H

#include "Node.h"

struct BinaryTreeNode {
	struct Node* node;

	struct BinaryTreeNode* left;
	struct BinaryTreeNode* right;
};

struct BinaryTree {
	struct BinaryTreeNode* root;
	
	int (*cmp)(void* data1, void* data2);

	void (*insert)(void* data, int dataLen, struct BinaryTree* tree);
	void (*remove)(void* data, struct BinaryTree* tree);
	void* (*search)(void* data, struct BinaryTree* tree);
};

struct BinaryTree* binary_tree_constructor(int (*cmp)(void* data1, void* data2));
void binary_tree_deconstructor(struct BinaryTree* tree);

#endif