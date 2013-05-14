#include <stdlib.h>
#include "queues.h"

typedef struct _element {
	Item* item;
	struct _element* next;
	struct _element* prev;
} Element;

struct _queue {
	Element* front;
	Element* back;
};

Queue* CreateQueue() {
	Queue* queue = (Queue*) malloc(sizeof(Queue));
	queue->front = NULL;
	queue->back = NULL;
	return queue;
}

void PushQ(Queue* queue, Item* item) {
	if (queue == NULL) return;
	Element* element = (Element*) malloc(sizeof(Element));
	Element* last = queue->back;
	element->next = last;
	element->prev = NULL;
	queue->back = element;
	if (last != NULL) {
		last->prev = element;
	} else {	//means front is also null
		queue->front = element;
	}
	element->item = item;
}

Item* PopQ(Queue* queue) {
	if (IsQueueEmpty(queue)) return NULL;
	Element* front = queue->front;
	Item* item = front->item;
	queue->front = queue->front->prev;
	if (queue->front != NULL) queue->front->next = NULL;
	free(front);
	return item;
}

void ClearQueue(Queue* queue) {
	while (PopQ(queue) != NULL);
}

int IsQueueEmpty(Queue* queue) {
	return (queue->front == NULL);
}

int IsInQueue(Queue* queue, Item* item) {
	Element* e = queue->front;
	while (e != NULL) {
		if (item == e->item) return 1;
		e = e->prev;
	}
	return 0;
}
