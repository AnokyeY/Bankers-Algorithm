/*
 queue.h

FullName: Dwumah Anokye
Course ID: EECS 3221 Section Z

Header file for queue.c
includes the declarations for queue structure along with the methods
needed to manage a queue

Queue is implemented using a linked list

 * */
#include "Activity.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef QUEUE_H_
#define QUEUE_H_


typedef struct queue{
	Activity *head;
	Activity *tail;
	int size;
} queue;

void initialize(queue *q);
int isempty(queue *q);
void enqueue(queue *q, Activity *act);
void  dequeue(queue *q) ;
Activity * Top(queue *q);

#endif /* QUEUE_H_ */
