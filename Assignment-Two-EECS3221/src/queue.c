/*
 queue.c

FullName: Dwumah Anokye
Course ID: EECS 3221 Section Z

This c file implements the methods needed for a queue as well as
its constructor

 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Activity.h"
#include "queue.h"

//initializes a given queue to its default values
void initialize(queue *q){
	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
}

//checks if the queue is empty
//return 0 if true non-zero otherwise
int isempty(queue *q) {
	return q->tail == NULL;
}

//add an activity to the back of an activity queue
void enqueue(queue *q, Activity *act) {

	Activity *tmp = NULL;
	tmp = act;
	tmp->next = NULL;
	if (!isempty(q)){
		q->tail->next = tmp;
		q->tail = tmp;
	}
	else {
		q->head = tmp;
		q->tail = tmp;
	}
	q->size = q->size +1;
}

//removes the first activity in the activity queue
void  dequeue(queue *q) {
	 if (!isempty(q)){
		
		 q->head = q->head->next;
		 q->size--;
		 
		 if (q->head == NULL){
			 q->tail=NULL;
		 }
	 }
	 else {
		 printf("\n queue is empty");
	 }
}

//returns a pointer the first element in the activity queue without removing it
Activity * Top(queue *q) {
	Activity *top= NULL;
	if ( !isempty(q) ){
		top = q->head;
	}
	else {
	}
	return top;
}

