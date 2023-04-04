/*
 Task.h

FullName: Dwumah Anokye
Course ID: EECS 3221 Section Z

Representation of a Task object

 * */
#include "queue.h"

#ifndef TASK_H
#define TASK_H

typedef struct Task{
	int task_Number;
	int delay;
	int resource_type;
	int initial_claim;
	int time_taken;
	int waiting_time;
	char status[20];
	int resources_assigned[30];
	int resources_needed[30];
	int released[30]; //how much of each resource type released in a cycle
	queue priority_queue; //priority queue of a tasks activities to be completed
}Task;


#endif
