#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Node.h"

struct Node* node_constructor(void* data, int dataLen) {
	struct Node* node = (struct Node*)calloc(1, sizeof(struct Node));
	if (!node) {
		fprintf(stderr, "[-] error allocating memory for node\n");
		return NULL;
	}

	node->data = calloc(1, dataLen);
	if (!node->data) {
		fprintf(stderr, "[-] error allocating memory for data\n");
		free(node);
		return NULL;
	}
	memcpy(node->data, data, dataLen);

	node->dataLen = dataLen;

	return node;
}

void node_deconstructor(struct Node* node) {
	free(node->data);
	free(node);
}

void copy_node_data(struct Node* dest, struct Node* src) {
	if (dest->dataLen == src->dataLen) {
		memcpy(dest->data, src->data, dest->dataLen);
	}
	else {
		dest->data = realloc(dest->data, src->dataLen);

		if (!dest->data) {
			fprintf(stderr, "[-] error reallocating destination data\n");
			return;
		}
		
		dest->dataLen = src->dataLen;

		memset(dest->data ,0, dest->dataLen);
		memcpy(dest->data, src->data, dest->dataLen);
	}
}
