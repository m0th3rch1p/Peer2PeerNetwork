#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Queue.h"

void insert_queue(void* data, int dataLen, struct Queue* queue);
void pop_queue(struct Queue* queue);
void* peek_queue(struct Queue* queue);

struct Queue* queue_constructor(int (*cmp)(void* data1, void* data2)) {
	struct Queue* queue = (struct Queue*)calloc(1, sizeof(struct Queue));
	if (!queue) {
		fprintf(stderr, "[-] error allocating memory for queue.\n");
		return NULL;
	}

	queue->queue = linked_list_constructor(cmp);
	if (!queue->queue) {
		free(queue);
		return NULL;
	}

	queue->insert = insert_queue;
	queue->peek = peek_queue;
	queue->pop = pop_queue;

	return queue;
}

void queue_deconstrutor(struct Queue* queue) {
	linked_list_deconstructor(queue->queue);
	free(queue);
}

void insert_queue(void* data, int dataLen, struct Queue* queue) {
	queue->queue->insert(0, data, dataLen, queue->queue);
}

void pop_queue(struct Queue* queue) {
	queue->queue->remove(0, queue->queue);
}

void* peek_queue(struct Queue* queue) {
	return queue->queue->search(0, queue->queue);
}