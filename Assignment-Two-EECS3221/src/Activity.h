/*
 Activity.h

FullName: Dwumah Anokye
Course ID: EECS 3221 Section Z

Representation of an activity

 * */
#ifndef ACTIVITY_H
#define ACTIVITY_H

typedef struct Activity{
		char action[20];
        int task_number;
        int delay;
        int resource_type;
        int hold;
	int start_time;
	struct Activity *next; //next activity to be processed after this one
} Activity;
#endif
