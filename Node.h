#ifndef NODE_H
#define NODE_H

struct Node {
	void* data;
	int dataLen;
};

struct Node* node_constructor(void* data, int dataLen);
void node_deconstructor(struct Node* node);

void copy_node_data(struct Node* dest, struct Node* src);
#endif