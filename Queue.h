#ifndef QUEUE_H
#define QUEUE_H

#include "LinkedList.h"

struct Queue {
	struct LinkedList* queue;

	int (*cmp)(void* data1, void* data2);

	void (*insert)(void* data, int dataLen, struct Queue* queue);
	void* (*peek)(struct Queue* queue);
	void (*pop)(struct Queue* queue);
};

struct Queue* queue_constructor(int (*cmp)(void* data1, void* data2));
void queue_deconstrutor(struct Queue* queue);

#endif