#include "stepqueue.h"

void initQueue(StepQueue *queue) {
	queue->first = NULL;
	queue->size = 0;
}

void enqueue(StepQueue *queue, Step item) {
	Step *new_node = malloc(sizeof(Step));

	*new_node = item;
	new_node->next = NULL;

	Step *head = queue->first;
	if (head == NULL) {
		queue->first = new_node;
	} else {
		while (head->next) {
			head = head->next;
		}
		head->next = new_node;
	}
	queue->size++;
}

void enqueueMultiple(StepQueue *queue, Step item, int count) {
	for (int i=0; i<count; i++) {
		enqueue(queue, item);
	}
}

Step dequeue(StepQueue *queue) {
	Step *tmp = queue->first;
	// TODO what to do if empty?
	Step ret;
	if( tmp == NULL) {
		ret.face = -1;
		ret.direction = 0;
		return ret;
	}
	queue->first = tmp->next;
	ret = *tmp;
	free(tmp);
	queue->size--;
	return ret;
}
