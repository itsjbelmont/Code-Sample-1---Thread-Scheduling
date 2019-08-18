/*	** Copyright Jack Belmont **
 * 
 * jOS Source Code
 * 
 * Headers and defines for multithreading
 * 
*/

#ifndef THREADS_H
#define THREADS_H

#include "types.h"
#include "jstdio.h"
#include "string.h"

#define MAX_THREADS 3
#define RUN_THREADS 3
#define STACKSIZE 1024
#define SLEEPTIME 300000

/* 
 * THREAD CONTROL BLOCK
 * tcb_t structure holds information pertaining to an individual thread. This allows
 * threads to context switch between completely independent thread tasks. tcb_t structures
 * link together as a doubly linked list to form a FIFO queue for round robin thread 
 * execution.
*/
typedef struct tcb {
	int id;								/* Identifier for the thread */
	//fileDescriptorTable_t *fdt;		/* File Descriptor table for files opened by thread*/
	bool assigned;						/* Is the thread currently assigned a task? */
	uint32_t *stackBase;				/* Base of stack used for local thread variables */
	uint32_t *stackPtr;					/* Stack pointer for local thread stack */
	int (*task)(void);					/* Function pointer the task is assigned to */
	struct tcb* next;					/* Next task waiting in ready queue */
	struct tcb* prev;					/* Previous task waiting in ready queue */
} tcb_t;


/* External Variables (Global to threads.c) */
extern tcb_t* runningThread;


/* Function Headers */
extern void contextSwitchThreads(uint32_t** oldStackPtr, uint32_t *newStackPtr);
extern void remapPIC();
extern uint32_t getEIP();
tcb_t* extractThread();
void addThread(tcb_t* thread);
void scheduler(void);
void startScheduler(void);
int threadCreate(void* stack, void* func);
void yeild(void);
void threadReturn(void);


#endif