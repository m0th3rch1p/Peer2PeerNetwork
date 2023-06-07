#ifndef LINKED_LIST
#define LINKED_LIST

#include "Node.h"

struct LinkedListNode {
	struct Node* node;
	struct LinkedListNode* next;
};

struct LinkedList {
	int length;
	struct LinkedListNode* head;

	void (*insert)(int index, void* data, int dataLen, struct LinkedList* list);
	void* (*search)(int index, struct LinkedList* list);
	void (*remove)(int index, struct LinkedList* list);
	void (*sort)(int (*cmp)(void* a, void* b), struct LinkedList* list);
};

struct LinkedList* linked_list_constructor();
void linked_list_deconstructor(struct LinkedList* list);
#endif