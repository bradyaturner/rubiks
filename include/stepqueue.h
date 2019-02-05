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
void enqueue(StepQueue *queue, Step item);
void enqueueMultiple(StepQueue *queue, Step item, int count);
Step dequeue(StepQueue *queue);
#endif
