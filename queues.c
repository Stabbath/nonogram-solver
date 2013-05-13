#include <stdlib.h>
#include "queues.h"

struct _element {
	Item* item;
	struct _element* next;
	struct _element* prev;
};

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
	Element* buffer = queue->back;
	queue->back = element;
	queue->back->next = buffer;
	buffer->prev = element;
	element->item = item;
}

Item* PopQ(Queue* queue) {
	if (IsQueueEmpty(queue)) return NULL;
	Element* front = queue->front;
	Item* item = front->item;
	queue->front = queue->front->prev;
	queue->front->next = NULL;
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
		e = e->next;
	}
	return 0;
}








/*
//after presolve + first solve: 16 unsolved
-????-
??????
#----#
######
#----#
??????
	//after guessing full: 8 unsolved
	-##---
	?--???
	#----#
	######
	#----#
	?--???
		//after guessing full: 3 unsolved
		-##---
		#---??
		#----#
		######
		#----#
		---#-?
			//after guessing full: 0 unsolved FINISHED
			-##---
			#---#-
			#----#
			######
			#----#
			---#-#
			//now tries guessing blank: 1 unsolved FUCKED UP
			-#???-
			#-??-?
			#----#
			######
			#----#
			?-????


*/























