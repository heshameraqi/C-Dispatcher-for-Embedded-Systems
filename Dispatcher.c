/********************************************************

 A Compact Task Dispatcher for Embedded Systems
  Ron Kreymborg Dispatcher Implemented By Hesham Eraqi

 hesham.eraqi@gmail.com
 November 2015
 
********************************************************/

#include "Dispatcher.h"

/********************************************************
 Definitions
********************************************************/
// #define Status_ReadyBit      (1<<0)  // A Ready  Task
// #define Status_DelayBit      (1<<1)  // A Delay Task
// #define Status_BusyBit       (1<<2)  // Either Ready or Delay Task

/********************************************************
 Datatypes
********************************************************/
struct {
	// char status;        // Ready, Delay, and Busy bits (from LSB) (Task status is never used in the paper functions !)
	int  delay;            // Delay in number of ticks
	void (*pntr)();        // A pointer to task function
	char next_id;          // Next task pointer
} tasks[TOTALTASKS+1];     // The "+1" is for the first unused task

/********************************************************
 Global Variables
********************************************************/
char ReadyQueueTail_id;  // The ready queue tail (head is : tasks[0].next)
char DelayQueueHead_id;  // The delay queue head (tail i is detected by iterating the queue and finding that tasks[i].next = 0)
char RunningTask_id;     // The id for the task to be executed by RunTask
char NewTask_id;         // The id for the task that DoQueTask add to the ready queue
int NewDelay;            // ReRunMe delay that DoQueDelay put into delay element of the task

/********************************************************
 Local Functions Declarations (Scope of this file only)
********************************************************/
void DoQueTask();
void DoQueDelay();
char GetNewTask(void (*pntr)()); // Returns 0 if no task entry is available (As Ron Kreymborg in his "Possible Extensions" section)
void RunTask();

/********************************************************
 Functions Definitions
********************************************************/
void InitMulti() {

	int i;
	for (i=0; i<=TOTALTASKS; i++) {
		tasks[i].pntr = 0; // It is used by the GetNewTask function
		tasks[i].next_id = 0; // Used to know the tail of the delay queue
	}
	ReadyQueueTail_id = 0; // This can tell if the ready queue is empty or not
	DelayQueueHead_id = 0; // This can tell if the delay queue is empty or not
}

void DecrementDelay() {

	if (DelayQueueHead_id) {                                               // If the Delay Queue is not empty (there is a head for it)
		tasks[DelayQueueHead_id].delay--;                                  // Decrement the delay of the delay queue head
		while (DelayQueueHead_id && tasks[DelayQueueHead_id].delay == 0) { // IF there is a new head and it's delay reached zero
			NewTask_id = DelayQueueHead_id;                                // Set NewTask_id
			DelayQueueHead_id = tasks[DelayQueueHead_id].next_id;          // Set head to next (This resets the head if last task in the queue is executed)
			DoQueTask();                                                   // Copy the task to the Ready Queue
		}
	}
}

void QueTask(void (*pntr)()) {

	if (GetNewTask(pntr) != 0) { // If NewTask_id is set successfully (tasks[NewTask_id].pntr and .next_id is set too)
		DoQueTask();
	}
}

/*
 * This function uses NewTask_id to insert a new task to the delay queue
 * Sets tasks[NewTask_id].next_id and insure ready queue is linked fine 
*/
void DoQueTask() {
   
	if (ReadyQueueTail_id == 0) // If the ready queue is empty
		tasks[0].next_id = NewTask_id;
	else
		tasks[ReadyQueueTail_id].next_id = NewTask_id;
     
	ReadyQueueTail_id = NewTask_id;           // Set NewTask_id as ReadyQueueTail
	tasks[NewTask_id].next_id = 0;            // Set tail next to zero

	NewTask_id = 0; // Reset NewTask_id so that RunTask can check if ReRunMe is required (if NewTask_id == RunningTask_id)
}

/*
 * This function should only be called from the task that is supposed to rerun (re-execute)
*/
void ReRunMe(int delay) {

   NewDelay = delay;            // Set NewDelay (RunTask function calls DoQueDelay that uses NewDelay)
   NewTask_id = RunningTask_id; // Set NewTask_id after DoQueTask has reset it, so that RunTask knows about that ReRunMe is called
}

/*
 * This function should  set the NewTask_id variable with the first availabe (empty) entry of the linked list queue
 * This function is modified as suggested by Ron Kreymborg in his "Possible Changes and Extensions" section.
 * It will return false (0) if the task is already in the ready or delay queues, or there is no availabe entry for new task.
*/
char GetNewTask(void (*pntr)()) {
  
  	char new_task_found = 0; // Boolean
	int i;
	for (i=1; i<= TOTALTASKS; i++) {
		if (tasks[i].pntr == 0 && !new_task_found) {
			new_task_found = 1;
			NewTask_id = i;
			tasks[NewTask_id].pntr = pntr;
		}
		else if (tasks[i].pntr == pntr) {
			return 0;
		}
	}

	if (new_task_found)
		return 1;
	else
		return 0;
}

void Dispatch() {

	if (tasks[0].next_id) {                                // If there is a head (not empty ready queue)
		RunningTask_id = tasks[0].next_id;                 // Set the RunningTask_id to the ready queue head
		tasks[0].next_id = tasks[RunningTask_id].next_id;  // New head is the next of the previous head
		if (tasks[0].next_id == 0)                         // Reset ready queue tail if ready queue was only one task
			ReadyQueueTail_id = 0;
		RunTask();                         		           // Run the RunningTask_id
	}
}

void RunTask() {

	(*tasks[RunningTask_id].pntr)();        // Run the RunningTask_id

	tasks[RunningTask_id].next_id = 0;      // Because next is used to check stop when iterating the delay queue
	if (NewTask_id == RunningTask_id) {     // If ReRunMe is required
		if (NewDelay) 	                    // If a NewDelay of non-zero value was required by ReRunMe
			DoQueDelay();                   // Uses NewDelay
		else
			DoQueTask();
	}
	else
		tasks[RunningTask_id].pntr = 0;     // Free that entry in the linked list (used by GetNewTask)

	// RunningTask_id = 0; // Redundant Step
}

/*
 * This function uses NewDelay and NewTask_id to insert a new task to the delay queue
 * Sets tasks[NewTask_id].delay and next_id and insure delay queue is linked fine 
*/
void DoQueDelay() {

	int IntVal = NewDelay;
	char Pntr0 = DelayQueueHead_id;
	char Pntr1 = 0;
	int IntVal_old = 0; // To be used in case a negative value of IntVal is obtained

	if (DelayQueueHead_id == 0) { // If delay queue is empty
		DelayQueueHead_id = NewTask_id;
		tasks[NewTask_id].delay = NewDelay;
		tasks[NewTask_id].next_id = 0;
	}
	else {
		while (1) {
			IntVal -= tasks[Pntr0].delay;
			if (IntVal < 0) {   // The entry should be inserted after Pntr1
				tasks[NewTask_id].next_id = Pntr0;
                if (Pntr1)
                    tasks[Pntr1].next_id = NewTask_id;
                else {          // Else, node should not be inserted at the start of queue
                    DelayQueueHead_id = NewTask_id;
                    IntVal_old = NewDelay;
                }
				tasks[NewTask_id].delay = IntVal_old;

				// Decrease all the next delays accordingly
				while (Pntr0) {
					tasks[Pntr0].delay -= IntVal_old;
                    Pntr0 = tasks[Pntr0].next_id;
				}
				break;
			}
            else if (tasks[Pntr0].next_id == 0) { // End of delay queue is reached
				tasks[Pntr0].next_id = NewTask_id;
				tasks[NewTask_id].delay = IntVal;
				tasks[NewTask_id].next_id = 0;
				break;
			}
            
			IntVal_old = IntVal;
			Pntr1 = Pntr0;
			Pntr0 = tasks[Pntr0].next_id;
		}
	}

	NewTask_id = 0; // Reset NewTask_id so that RunTask can check if ReRunMe is required (if NewTask_id == RunningTask_id)
}

void QueDelay(void (*pntr)(), int delay) {

   if (GetNewTask(pntr) != 0) { // If NewTask_id is set successfully (tasks[NewTask_id].pntr and .next_id is set too)
		if (delay) {         // If delay is not zero
			NewDelay = delay;   // To beused by DoQueDelay
			DoQueDelay();       // Put the task on the delay queue
		}
		else
			DoQueTask();        // Put the task on the ready queue
	}
}
