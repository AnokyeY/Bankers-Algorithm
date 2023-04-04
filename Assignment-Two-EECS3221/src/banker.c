/*
 *  Full Name:Dwumah Anokye
 *  Course ID: EECS 3221 Section Z
 *  Description of the program:
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Task.h"
#include "Resource.h"
#include "Activity.h"
#include "Bankers.h"





int main(int argc, char *argv[]){
	int numberOfTasks,numberOfResourceTypes,unitsPresent;

	FILE *fp;
	fp = fopen(argv[1],"r"); //opening the input file in read mode
	if(fp != NULL){
		printf("\tFIFO");
		fscanf(fp,"%d %d %d",&numberOfTasks,&numberOfResourceTypes,&unitsPresent);

		//creates resource objects needed
		Resource *resource_queue =malloc(numberOfResourceTypes*sizeof(Resource));
		for (int j =0; j<numberOfResourceTypes; j++){
			resource_queue[j].resource_type = j+1;
			resource_queue[j].units_available = unitsPresent;
		}

		//Creates task objects needed
		Task *task_queue=malloc(numberOfTasks*sizeof(Task));
		for (int j=0; j< numberOfTasks; j++){
			task_queue[j].task_Number = j+1;
			task_queue[j].time_taken = 0;
			task_queue[j].waiting_time = 0;
			strcpy(task_queue[j].status,"ok");
			initialize(&task_queue[j].priority_queue);
		}

		/*initializes arrays to keep track of resource types
		 * */
		for (int j=0; j<numberOfTasks;j++){
			for (int k=0; k<30; k++) {
				task_queue[j].resources_needed[k] = 0;
				task_queue[j].resources_assigned[k] = 0;
				task_queue[j].released[k] =0;
				}
		}


		/*Read Me
		 * Activities provided in file have to be less than 100
		 * Creates Activity objects and initiates them to the activities provided in input file
		 * */
		Activity *activity_queue=malloc(100*sizeof(Activity));
		int numberOfActivities=0;
		int i=0;
		while ( !feof(fp)){
			fscanf(fp,"%s %d %d %d %d",activity_queue[i].action,
					&activity_queue[i].task_number,
					&activity_queue[i].delay,
					&activity_queue[i].resource_type,
					&activity_queue[i].hold);
			i= i+1;
			numberOfActivities = numberOfActivities+1;
//			if (feof(fp)) {
//				break;
//			}
		}


		//puts activities associated with each task in the task's activity queue
		for (int j =0; j<numberOfTasks; j++){
			for (int l =0; l<numberOfActivities; l++){
				if ( task_queue[j].task_Number == activity_queue[l].task_number) {
					enqueue(&task_queue[j].priority_queue,&activity_queue[l]);
				}
			}
		}






		/*Algorithm for Optimistic resource manager
		*
		*
		*add a better measure for when to end algorithm
		*/
		int finished =0;
		int aborted_tasks =0;
		int cycle =0;
		int total =0;
		int total_wait=0; //total time spent waiting
		while ( (finished+aborted_tasks) != numberOfTasks  ) {

			//resources to be released at the end of each cycle
			int *toBeReleased = malloc(numberOfResourceTypes*sizeof(int));
			for (int j=0; j< numberOfResourceTypes; j++) {
				toBeReleased[j] = 0;
			}

			//resources to be released during deadlocks
			int *toBeReleasedDeadlock = malloc(numberOfResourceTypes*sizeof(int));//resources to be released at the end of each cycle
			for (int j=0; j< numberOfResourceTypes; j++) {
							toBeReleasedDeadlock[j] = 0;
			}

			//resources that each tasks releases in a cycle
			for(int j=0; j< numberOfTasks;j++) {
				for (int k=0;k<30;k++){
						task_queue[j].released[k] =0;
					}
				}

			/*checking for deadlocks ahead of this cycle
			 */
			int deadlocked = 1;//1 if deadlocked 0 if not
			int deadlocked_tasks = 0;
			int terminated_tasks = 0;
			while ( deadlocked != 0){
				deadlocked_tasks = 0;
				terminated_tasks = 0;
				for (int j=0;j<numberOfTasks; j++){
					if ( strcmp(task_queue[j].status,"terminated") ==0){
						terminated_tasks = terminated_tasks +1;
					}
				}

				//determining how much task are deadlocked
				for (int j=0; j<numberOfTasks; j++){
					if ( strcmp(task_queue[j].status,"terminated") !=0
						&& strcmp(Top(&task_queue[j].priority_queue)->action,"request") == 0
						&& Top(&task_queue[j].priority_queue)->hold > (resource_queue[Top(&task_queue[j].priority_queue)->resource_type-1].units_available
								+ toBeReleasedDeadlock[Top(&task_queue[j].priority_queue)->resource_type-1]) ){
						deadlocked_tasks = deadlocked_tasks +1;
					}
				}

				//system is deadlocked
				if ( deadlocked_tasks>0 &&(deadlocked_tasks) == (numberOfTasks - terminated_tasks) ){

					//finding lowest task-numbered non terminated non aborted task to abort
					int min_idx =-1;
					int idx =0;
					while (min_idx == -1 && idx!=numberOfTasks){

						for (int k=0; k <numberOfTasks; k++){
							if ( (task_queue[k].task_Number == idx+1)
									&&(strcmp(task_queue[k].status,"aborted") !=0 && strcmp(task_queue[k].status,"terminated") !=0 ) ) {
									min_idx = k;
									break;
							}
						}
						idx = idx+1;
					}

					//lowest task found time to abort
					if (min_idx != -1){
						for(int l=0; l<numberOfResourceTypes; l++){
							toBeReleasedDeadlock[l] = toBeReleasedDeadlock[l] +task_queue[min_idx].resources_assigned[l];
							toBeReleased[l] = toBeReleased[l] +task_queue[min_idx].resources_assigned[l];
							task_queue[min_idx].resources_assigned[l] = 0;
						}

						task_queue[min_idx].time_taken = 0;
						task_queue[min_idx].waiting_time = 0;
						strcpy(task_queue[min_idx].status,"aborted");
						aborted_tasks = aborted_tasks +1;
						printf("\nDeadlock detected aborted lowest task:%d and releasing its resources",task_queue[min_idx].task_Number);
					}
				}
				else {
					deadlocked =0; //System is not deadlocked break while loop
				}

			}

			//processing each tasks list of activities in FIFO manner
			for(int j=0;j <numberOfTasks; j++){

				//initiate clause
				if ( strcmp(Top(&task_queue[j].priority_queue)->action, "initiate") == 0 && strcmp(task_queue[j].status,"aborted") != 0){

					//initiating tasks
					task_queue[j].delay = Top(&task_queue[j].priority_queue)->delay;
					task_queue[j].initial_claim = Top(&task_queue[j].priority_queue)->hold;
					task_queue[j].resource_type = Top(&task_queue[j].priority_queue)->resource_type;

					//removing this activity from task priority queue
					dequeue(&task_queue[j].priority_queue);
				}

				//request clause
				else if ( strcmp(Top(&task_queue[j].priority_queue)->action,"request") == 0 && strcmp(task_queue[j].status,"aborted") != 0) {

					//delay is over activity is ready to be processed
					if ( Top(&task_queue[j].priority_queue)->delay == 0 ) {

						//request granted
						if (Top(&task_queue[j].priority_queue)->hold <= resource_queue[Top(&task_queue[j].priority_queue)->resource_type-1].units_available ){

							//adding to resources assigned for task
							task_queue[j].resources_assigned[Top(&task_queue[j].priority_queue)->resource_type-1] =
									task_queue[j].resources_assigned[Top(&task_queue[j].priority_queue)->resource_type-1] +
									Top(&task_queue[j].priority_queue)->hold;

							//removing requested units from available units
							resource_queue[Top(&task_queue[j].priority_queue)->resource_type-1].units_available =
									resource_queue[Top(&task_queue[j].priority_queue)->resource_type-1].units_available - Top(&task_queue[j].priority_queue)->hold;

							//removing this activity from task priority queue
							dequeue(&task_queue[j].priority_queue);
						}
						else{//request could not be granted (i.e blocked)
							strcpy(task_queue[j].status,"blocked");
							task_queue[j].waiting_time = task_queue[j].waiting_time+1;

						}
					}

					else if (Top(&task_queue[j].priority_queue)->delay >0) { //task is still on delay
						Top(&task_queue[j].priority_queue)->delay = Top(&task_queue[j].priority_queue)->delay -1;
					}
				}

				//release clause
				else if (strcmp(Top(&task_queue[j].priority_queue)->action,"release") == 0 && strcmp(task_queue[j].status,"aborted") != 0) {

					//time for release delay is done
					if ( Top(&task_queue[j].priority_queue)->delay == 0 ) {

						//removing units from task resources held
						task_queue[j].resources_assigned[Top(&task_queue[j].priority_queue)->resource_type-1] =
															task_queue[j].resources_assigned[Top(&task_queue[j].priority_queue)->resource_type-1] -
															Top(&task_queue[j].priority_queue)->hold;

						//adjusting units of each resource type to be released at end of cycle
						toBeReleased[Top(&task_queue[j].priority_queue)->resource_type-1] =
															toBeReleased[Top(&task_queue[j].priority_queue)->resource_type-1] + Top(&task_queue[j].priority_queue)->hold;

						//removing this activity from task priority queue
						dequeue(&task_queue[j].priority_queue);
					}

					//task is still on delay
					else if (Top(&task_queue[j].priority_queue)->delay >0) {
						Top(&task_queue[j].priority_queue)->delay = Top(&task_queue[j].priority_queue)->delay -1;
					}
				}

				//terminate clause
				if ( strcmp(Top(&task_queue[j].priority_queue)->action,"terminate") == 0 && Top(&task_queue[j].priority_queue)->hold==0
						&& strcmp(task_queue[j].status,"aborted") != 0){

					//terminate still has not finished its delay
					if (Top(&task_queue[j].priority_queue)->delay >0) {
						Top(&task_queue[j].priority_queue)->delay = Top(&task_queue[j].priority_queue)->delay-1;
					}

					//terminate has finished its delay and terminate activitiy can now be processed
					else{
					strcpy(task_queue[j].status,"terminated");
					task_queue[j].time_taken = cycle +1;
					total = total + task_queue[j].time_taken;
					Top(&task_queue[j].priority_queue)->hold = Top(&task_queue[j].priority_queue)->hold +1;

					//incrementing finished tasks
					finished = finished +1;
					}
				}

			}
				//releasing to resources available at end of cycle
				for (int j=0; j< numberOfResourceTypes; j++){
					resource_queue[j].units_available = resource_queue[j].units_available +toBeReleased[j];
				}

				//moving blocked tasks to beginning so they can be checked first next iteration
				for (int j = numberOfTasks-1; j>=0; j--){
					for (int k=numberOfTasks-1; k>0;k--){
						if ( strcmp(task_queue[k].status,"blocked") == 0 && strcmp(task_queue[k-1].status,"blocked") != 0 ){
							Task tmp = task_queue[k];
							task_queue[k] = task_queue[k-1];
							task_queue[k-1] = tmp;

						}
					}
				}


				free(toBeReleasedDeadlock);
				free(toBeReleased);
				cycle = cycle +1;

			}
		// sorting Tasks using Selection sort
		for (int j=0 ;j<numberOfTasks-1;j++){
			int min_idx = j;
			for (int k=j+1; k<numberOfTasks;k++){
				if (task_queue[k].task_Number < task_queue[min_idx].task_Number){
					min_idx = k;
				}
			}
			Task temp = task_queue[min_idx];
			task_queue[min_idx] = task_queue[j];
			task_queue[j] = temp;
		}

		/*Printing output for FIFO optimistic resource manager
		 * */

		for (int j=0;j< numberOfTasks;j++) {
			if (strcmp(task_queue[j].status,"aborted") != 0){
				total_wait = total_wait + task_queue[j].waiting_time;
				printf("\nTask %d\t%d\t%d\t%2.0f%%",
						task_queue[j].task_Number,
						task_queue[j].time_taken,
						task_queue[j].waiting_time,
						((double)(task_queue[j].waiting_time)/ (double)(task_queue[j].time_taken))*100 //percentage of time task spent waiting
					);
			}
			else {
				printf("\nTask %d\taborted",task_queue[j].task_Number);
			}
		}
		printf("\ntotal \t%d\t%d\t\%2.0f%%",total,total_wait,
				((double)(total_wait) / (double)(total)) *100 //total percentage of time spent waiting
				);

		free(activity_queue);
	
		free(task_queue);
		free(resource_queue);

	}
	fclose(fp);
	

	/*Bankers Algorithm Section
			 * */
	FILE *fp2;
	fp2 = fopen(argv[1],"r"); //opening the input file in read mode
	//fp2 = fopen("input-08","r");
		if(fp2 != NULL){
		//	printf("\n\tBANKER'S");
			fscanf(fp2,"%d %d %d",&numberOfTasks,&numberOfResourceTypes,&unitsPresent);

			//setting existence vector
			int Existence[numberOfResourceTypes];
			for (int j=0; j< numberOfResourceTypes; j++){
				Existence[j] = unitsPresent;
			}

			//creates resource objects needed
			Resource *resource_queue =malloc(numberOfResourceTypes*sizeof(Resource));
			for (int j =0; j<numberOfResourceTypes; j++){
				resource_queue[j].resource_type = j+1;
				resource_queue[j].units_available = unitsPresent;
			}

			//Creates task objects needed
			Task *task_queue=malloc(numberOfTasks*sizeof(Task));
			for (int j=0; j< numberOfTasks; j++){
				task_queue[j].task_Number = j+1;
				task_queue[j].time_taken = 0;
				task_queue[j].waiting_time = 0;
				strcpy(task_queue[j].status,"ok");
				initialize(&task_queue[j].priority_queue);
			}


			/*README the limit of resource types allowed is 30
			 * */
			for (int j=0; j<numberOfTasks;j++){
				for (int k=0; k<30; k++) {
					task_queue[j].resources_needed[k] = 0;
					task_queue[j].resources_assigned[k] = 0;
					task_queue[j].released[k] =0;
				}
			}

			/*Read Me
			 * Activities provided in file have to be less than 100
			 * */
			Activity *activity_queue=malloc(100*sizeof(Activity));
			//Creates Activity objects and initiates them to the activities provided in input file
			int numberOfActivities=0;
			int i=0;
			while ( !feof(fp2)){
				fscanf(fp2,"%s %d %d %d %d",activity_queue[i].action,
						&activity_queue[i].task_number,
						&activity_queue[i].delay,
						&activity_queue[i].resource_type,
						&activity_queue[i].hold);
				i= i+1;
				numberOfActivities = numberOfActivities+1;
	//			if (feof(fp)) {
	//				break;
	//			}
			}


			//puts activities associated with each task in the task's activity queue
			for (int j =0; j<numberOfTasks; j++){
				for (int l =0; l<numberOfActivities; l++){
					if ( task_queue[j].task_Number == activity_queue[l].task_number) {
						enqueue(&task_queue[j].priority_queue,&activity_queue[l]);
					}
				}
			}


			int terminated_tasks = 0;
			int aborted_tasks =0;
			int cycle =0;
			int total =0;
			int total_wait=0; //total time spent waiting
			/*Resource management using bankers algorithm
			*/
			while ( (aborted_tasks+terminated_tasks) != numberOfTasks  && cycle != 200) {

				//resources to be released for each resource type this cycle
				int *toBeReleased = malloc(numberOfResourceTypes*sizeof(int));//resources to be released at the end of each cycle
				for (int j=0; j< numberOfResourceTypes; j++) {
					toBeReleased[j] = 0;
				}

				//resources to be released for each task at end of cycle
				for(int j=0; j< numberOfTasks;j++) {
					for (int k=0;k<30;k++){
						task_queue[j].released[k] =0;
					}
				}

				//processing an activity for each task during a cycle
				for(int j=0;j <numberOfTasks; j++){

					//initiate clause
					if ( strcmp(Top(&task_queue[j].priority_queue)->action, "initiate") == 0 && strcmp(task_queue[j].status,"aborted") != 0){

						//initial claim is greater than units in existence for that resource aborting task
						if (Top(&task_queue[j].priority_queue)->hold >Existence[Top(&task_queue[j].priority_queue)->resource_type-1]){
                                                        printf("\nBanker aborts task:%d before run begins: claim for resource %d (%d) exceeds number of units present (%d)",task_queue[j].task_Number,Top(&task_queue[j].priority_queue)->resource_type,Top(&task_queue[j].priority_queue)->hold,Existence[Top(&task_queue[j].priority_queue)->resource_type-1] );

							for(int l=0; l<numberOfResourceTypes; l++){
								toBeReleased[l] = toBeReleased[l] +task_queue[j].resources_assigned[l];
								}
							task_queue[j].time_taken = 0;
							task_queue[j].waiting_time = 0;
							strcpy(task_queue[j].status,"toBeAborted");
							terminated_tasks = terminated_tasks+1;
						}
						//initiating task
						else{
							task_queue[j].delay = Top(&task_queue[j].priority_queue)->delay;
							task_queue[j].initial_claim = Top(&task_queue[j].priority_queue)->hold;
							task_queue[j].resource_type = Top(&task_queue[j].priority_queue)->resource_type;


						}

						//removing current activity for task
						dequeue(&task_queue[j].priority_queue);

					}

					//request clause
					else if ( strcmp(Top(&task_queue[j].priority_queue)->action,"request") == 0 && strcmp(task_queue[j].status,"aborted") != 0) {

						//activity is done its delay and is ready to be processed
						if ( Top(&task_queue[j].priority_queue)->delay == 0 ) {

							//task request more than its needs aborted
							if (Top(&task_queue[j].priority_queue)->hold > task_queue[j].resources_needed[Top(&task_queue[j].priority_queue)->resource_type-1]){
								for(int l=0; l<numberOfResourceTypes; l++){
									toBeReleased[l] = toBeReleased[l] +task_queue[j].resources_assigned[l];
								}
								printf("\nDuring cycle %d-%d of Banker's algorithms Task %d's request exceeds its claim; aborted; %d units available next cycle",cycle,cycle+1,task_queue[j].task_Number,task_queue[j].resources_assigned[Top(&task_queue[j].priority_queue)->resource_type-1]);
								task_queue[j].time_taken = 0;
								task_queue[j].waiting_time = 0;
								strcpy(task_queue[j].status,"toBeAborted");
								aborted_tasks = aborted_tasks +1;
							}
							else{
								//setting resources assigned
								task_queue[j].resources_assigned[Top(&task_queue[j].priority_queue)->resource_type-1] =
								task_queue[j].resources_assigned[Top(&task_queue[j].priority_queue)->resource_type-1] +
								Top(&task_queue[j].priority_queue)->hold;

								//removing from available
								resource_queue[Top(&task_queue[j].priority_queue)->resource_type-1].units_available =
										resource_queue[Top(&task_queue[j].priority_queue)->resource_type-1].units_available - Top(&task_queue[j].priority_queue)->hold;

								//using bankers algorithm to determine whether request is safe
								int safe = BankersAlgorithm(task_queue,Existence,numberOfResourceTypes,numberOfTasks,terminated_tasks,aborted_tasks) ;

								//request granted
								if (safe == 1 ){


									//removing current activity for task
									dequeue(&task_queue[j].priority_queue);


								}
								else{ //request could not be granted (i.e blocked)

									//reversing request since state was not safe
									//setting resources assigned
									task_queue[j].resources_assigned[Top(&task_queue[j].priority_queue)->resource_type-1] =
										task_queue[j].resources_assigned[Top(&task_queue[j].priority_queue)->resource_type-1] -
										Top(&task_queue[j].priority_queue)->hold;
									//restoring available
									resource_queue[Top(&task_queue[j].priority_queue)->resource_type-1].units_available =
										resource_queue[Top(&task_queue[j].priority_queue)->resource_type-1].units_available + Top(&task_queue[j].priority_queue)->hold;

								strcpy(task_queue[j].status,"blocked");
								task_queue[j].waiting_time = task_queue[j].waiting_time+1;
								}
							}
						}
						//activity is still on delay
						else if (Top(&task_queue[j].priority_queue)->delay >0) {
							Top(&task_queue[j].priority_queue)->delay = Top(&task_queue[j].priority_queue)->delay -1;

						}

					}
					//release clause
					else if (strcmp(Top(&task_queue[j].priority_queue)->action,"release") == 0 && strcmp(task_queue[j].status,"aborted") != 0) {

						//time for release activities delay is zero
						if ( Top(&task_queue[j].priority_queue)->delay == 0 ) {

							//setting resources to be released at end of cycle
							task_queue[j].released[Top(&task_queue[j].priority_queue)->resource_type-1] = Top(&task_queue[j].priority_queue)->hold;
							toBeReleased[Top(&task_queue[j].priority_queue)->resource_type-1] =
																toBeReleased[Top(&task_queue[j].priority_queue)->resource_type-1] + Top(&task_queue[j].priority_queue)->hold;

							//removing current activity for task
							dequeue(&task_queue[j].priority_queue);
						}

						//activity still on delay
						else if (Top(&task_queue[j].priority_queue)->delay >0) {
							Top(&task_queue[j].priority_queue)->delay = Top(&task_queue[j].priority_queue)->delay -1;
						}

					}

					//terminate clause
					if ( strcmp(Top(&task_queue[j].priority_queue)->action,"terminate") == 0 && Top(&task_queue[j].priority_queue)->hold==0
							&& strcmp(task_queue[j].status,"aborted") != 0){

						//terminate activity still has not finished its delay
						if (Top(&task_queue[j].priority_queue)->delay >0) {
							Top(&task_queue[j].priority_queue)->delay = Top(&task_queue[j].priority_queue)->delay-1;
						}

						//terminate has finished its delay and terminate activitiy can now be processed
						else{
						strcpy(task_queue[j].status,"terminated");
						task_queue[j].time_taken = cycle +1;
						total = total + task_queue[j].time_taken;
						Top(&task_queue[j].priority_queue)->hold = Top(&task_queue[j].priority_queue)->hold +1;
						terminated_tasks = terminated_tasks +1;
						}
					}

				}
					//releasing to resouces availabe at end of cycle
					for (int j=0; j< numberOfResourceTypes; j++){
						resource_queue[j].units_available = resource_queue[j].units_available +toBeReleased[j];
					}
					//fixes each tasks inventory to show accurate resources held at end of cycle
					for (int j=0; j<numberOfTasks; j++){
						for (int l=0; l<30; l++){
							//removing units from task resources held
							task_queue[j].resources_assigned[l] =task_queue[j].resources_assigned[l] -
									task_queue[j].released[l];
							//setting  resources needed
							task_queue[j].resources_needed[l]=task_queue[j].initial_claim -
									task_queue[j].resources_assigned[l];
							if (strcmp(task_queue[j].status,"toBeAborted") == 0) {
								task_queue[j].resources_assigned[l] = 0;
								strcpy(task_queue[j].status,"aborted");
							}

						}
					}

					//moving blocked tasks to beginning so they can be checked first next cycle
					for (int j = numberOfTasks-1; j>=0; j--){
						for (int k=numberOfTasks-1; k>0;k--){
							if ( strcmp(task_queue[k].status,"blocked") == 0 && strcmp(task_queue[k-1].status,"blocked") != 0 ){
								Task tmp = task_queue[k];
								task_queue[k] = task_queue[k-1];
								task_queue[k-1] = tmp;

							}
						}
					}


					free(toBeReleased);
					cycle = cycle +1;

				}

			//sorting Tasks using Selection sort
			for (int j=0 ;j<numberOfTasks-1;j++){
				int min_idx = j;
				for (int k=j+1; k<numberOfTasks;k++){
					 if (task_queue[k].task_Number < task_queue[min_idx].task_Number){
					     min_idx = k;
					 }
				}
				 Task temp = task_queue[min_idx];
				 task_queue[min_idx] = task_queue[j];
				 task_queue[j] = temp;
			}

			/*Printing output for FIFO optimistic resource manager
			 * */
			printf("\n\tBANKER'S");
			for (int j=0;j< numberOfTasks;j++) {
				if (strcmp(task_queue[j].status,"aborted") != 0){
					total_wait = total_wait + task_queue[j].waiting_time;
					printf("\nTask %d\t%d\t%d\t%2.0f%%",
							task_queue[j].task_Number,
							task_queue[j].time_taken,
							task_queue[j].waiting_time,
							((double)(task_queue[j].waiting_time)/ (double)(task_queue[j].time_taken))*100 //percentage of time task spent waiting
						);
				}
				else {
					printf("\nTask %d\taborted",task_queue[j].task_Number);
				}
			}
			printf("\ntotal \t%d\t%d\t\%2.0f%%\n",total,total_wait,
					((double)(total_wait) / (double)(total)) *100 //total percentage of time spent waiting
					);

			free(activity_queue);
		
			free(task_queue);
			free(resource_queue);
		
		}
		fclose(fp2);
		

	return 0;
}

/*Uses bankers algorithm for multiple resources
 *return 1 if state is safe return 0 otherwise
 * */
int BankersAlgorithm(Task *task_queue, int *Existence, int numberOfResourceTypes, int numberOfTasks,int terminated_tasks, int aborted_tasks){
	int Allocated[numberOfResourceTypes];
	int Available[numberOfResourceTypes];
	int marked = numberOfResourceTypes +1;
	int completed =0;
	int Row = 0;//row that can be terminated

	//initializing allocated and available vector
	for (int j=0; j<numberOfResourceTypes;j++){
		Allocated[j]=0;
		Available[j] =0;
	}

	//setting currently Allocated vector
	for (int j=0; j<numberOfResourceTypes; j++){
		for (int k=0; k <numberOfTasks;k++){
			Allocated[j] = Allocated[j]+ task_queue[k].resources_assigned[j];
		}
	}

	//setting resources needed matrix
	for (int j = 0; j < numberOfTasks; j++) {
		task_queue[j].resources_needed[marked] =0; //setting all tasks to unmarked
	    for (int k = 0; k < numberOfResourceTypes; k++){
	    	task_queue[j].resources_needed[k] = task_queue[j].initial_claim - task_queue[j].resources_assigned[k];
	    }
	}

	//setting Available vector
	for (int j=0; j< numberOfResourceTypes; j++){
		Available[j] = Existence[j] - Allocated[j];
	}

	//checking for safe state
	while (Row != -1) {
		Row = -1;
		int f =0;// number of resources where request can be met

		//looking for a task that can terminate with the current available resources
		for (int j=0; j<numberOfTasks; j++){
			for ( int k=0; k<numberOfResourceTypes; k++){
				if (task_queue[j].resources_needed[k] <= Available[k]
					&&	task_queue[j].resources_needed[marked] != 1
					&& strcmp(task_queue[j].status, "terminated") != 0){
					f= f+1;
				}
			}

			//found task that can terminate
			if ( f == numberOfResourceTypes ){
				Row =j;
				break;
			}
		}
		//marking task as terminated and releasing its resources to available
		if ( Row != -1) {
			task_queue[Row].resources_needed[marked] = 1;
			completed = completed+1;

			//adding all its resource to available vector
			for(int j=0; j<numberOfResourceTypes; j++){
				Available[j] = Available[j] +task_queue[Row].resources_assigned[j];
			}

		}

		//did not find row that can terminate state might not be safe
		else {
			Row = -1;
		}
	}

	//state is safe
	if (completed == (numberOfTasks-terminated_tasks) ) {
		return 1;
	}

	//state is unsafe
	else{
		return 0;
	}
}

