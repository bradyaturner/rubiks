#ifndef STEPQUEUE_H
#define STEPQUEUE_H

#include <stdlib.h>

typedef struct stepItem {
	int face;
	int direction;
	struct stepItem *next;
} Step;

typedef struct stepQueue {
	Step *first;
	int size;
} StepQueue;

void initQueue(StepQueue *queue);
void enqueue(StepQueue *head, Step item);
Step dequeue(StepQueue *head);
#endif
