#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"

void insert_linked_list(int index, void* data, int dataLen, struct LinkedList* list);
void remove_linked_list(int index, struct LinkedList* list);
void* search_linked_list(int index, struct LinkedList* list);
void sort_linked_list(int (*cmp)(void* a, void* b), struct LinkedList* list);

struct LinkedListNode* linked_list_node_constructor(void* data, int dataLen);
void linked_list_node_deconstructor(struct LinkedListNode* node);

struct LinkedList* linked_list_constructor() {
	struct LinkedList* list = (struct LinkedList*)calloc(1, sizeof(struct LinkedList));
	if (!list) {
		fprintf(stderr, "[-] error allocating memory for linked list\n");
		return NULL;
	}

	list->sort = sort_linked_list;
	list->insert = insert_linked_list;
	list->remove = remove_linked_list;
	list->search = search_linked_list;
	
	return list;
}

void destroy_list_nodes(struct LinkedListNode* node) {
	if (node->next) destroy_list_nodes(node->next);
	linked_list_node_deconstructor(node);
}

void linked_list_deconstructor(struct LinkedList* list) {
	destroy_list_nodes(list->head);
	free(list);
}

struct LinkedList* iterate(int index, struct LinkedList* list) {
	if (index > list->length) {
		fprintf(stderr, "[-] index is out of bounds\n");
		return NULL;
	}

	struct LinkedListNode* cursor = list->head;
	while (index) {
		cursor = cursor->next;
		index--;
	}

	return cursor;
}

void insert_linked_list(int index, void* data, int dataLen, struct LinkedList* list) {
	struct LinkedListNode* node = linked_list_node_constructor(data, dataLen);
	
	if (!list->head) {
		list->head = node;
		list->length++;
		return;
	}
	else if (index == 0) {
		node->next = list->head;
		list->head = node;
		list->length++;
		return;
	}
	
	struct LinkedListNode* cursor = iterate((index - 1), list);
	cursor->next = node;

	list->length++;
}

void remove_linked_list(int index, struct LinkedList* list) {
	if (!list->head) {
		fprintf(stderr, "[-] cannot remove from empry list\n");
		return;
	}
	
	struct LinkedListNode* node_to_remove;
	if (index == 0) {
		node_to_remove = list->head;
		list->head = node_to_remove->next;
	}
	else {
		struct LinkedListNode* cursor = iterate((index - 1), list);
		node_to_remove = cursor->next;
		cursor->next = node_to_remove->next;
	}

	linked_list_node_deconstructor(node_to_remove);
	list->length--;
}

void* search_linked_list(int index, struct LinkedList* list) {
	if (index == 0) {
		return list->head->node->data;
	}

	struct LinkedListNode* cursor = iterate(index, list);
	return cursor->node->data;
}

struct LinkedListNode* linked_list_node_constructor(void* data, int dataLen) {
	struct LinkedListNode* node = (struct LinkedListNode*)calloc(1, sizeof(struct LinkedListNode));
	if (!node) {
		fprintf(stderr, "[-] error allocating memory for linked list node\n");
		return NULL;
	}

	node->node = node_constructor(data, dataLen);
	return node;
}

void linked_list_node_deconstructor(struct LinkedListNode* node) {
	node_deconstructor(node->node);
	free(node);
}

void sort_linked_list(int (*cmp)(void* a, void* b), struct LinkedList* list) {
	for (int i = 0; i < list->length; i++)
	{
		struct LinkedListNode* cursor_i = iterate(i, list);
		for (int n = i + 1; n < list->length; n++)
		{
			struct LinkedListNode* cursor_n = iterate(n, list);
			if (cmp(cursor_i->node->data, cursor_n->node->data) == 1) {
				void* tmp = cursor_n->node->data;
				cursor_n->node->data = cursor_i->node->data;
				cursor_i->node->data = tmp;
			}
		}
	}
}