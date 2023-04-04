/*
 * Bankers.h
 *
 * FullName: Dwumah Anokye
 * Course ID: EECS 3221 Section Z
 * header file for banker.c
 * includes the declaration of the banker's algorithm created in Banker.c
 */

#ifndef BANKERS_H_
#define BANKERS_H_

int BankersAlgorithm(Task *task_queue, int *Existence, int numberOfResourceTypes, int numberOfTasks,int terminated_tasks, int aborted_tasks);


#endif /* BANKERS_H_ */
