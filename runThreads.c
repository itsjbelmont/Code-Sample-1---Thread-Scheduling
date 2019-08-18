/*	** Copyright Jack Belmont **
 * 
 * jOS Source Code
 * 
 * Essentially a userspace program to run
 * 
*/

#include "runThreads.h"
#include "threads.h"
#include "jstdio.h"


/* Function pool */
static void (*funcs[])(void) = {func1, func2, func3};


/* Stack Pool */
static uint32_t stack1[STACKSIZE] __attribute__ ((aligned (4)));
static uint32_t stack2[STACKSIZE] __attribute__ ((aligned (4)));
static uint32_t stack3[STACKSIZE] __attribute__ ((aligned (4)));
uint32_t *stackPool[] = {stack1, stack2, stack3};


/* File Descriptor Table Pool
static fileDescriptorTable_t table1;
static fileDescriptorTable_t table2;
static fileDescriptorTable_t table3;
static fileDescriptorTable_t* fdtPool[] = {&table1, &table2, &table3};
*/


void func1(void) {
	int i = 0;
	
	while(1) {
		setPrintColor(GREEN);
		printf("1");
		busySleep(1000000);
		if (i++ >= 100) {
			break;
		}
	}
	
	threadReturn();
}


void func2(void){
	int i = 0;
	
	while(1) {
		setPrintColor(BLUE);
		printf("2");
		busySleep(1000000);
		if (i++ >= 100) {
			break;
		}
	}
	
	threadReturn();
}


void func3(void) {
	int i = 0;
	
	while(1) {
		setPrintColor(RED);
		printf("3");
		busySleep(1000000);
		if (i++ >= 100) {
			break;
		}
	}
	
	threadReturn();
}

void runThreads(void) {
	int i;
	
	/* Create threads from the thread pool */
	for (i = 0; i < RUN_THREADS; i++) {
		if (threadCreate(stackPool[i], funcs[i]) < 0) {
			perror("Cant create thread %d\n", i);
			break;
		}
	}
	
	startScheduler();
	
	setPrintColor(GRAY);
	printf("Finished Running Threads\n");
	
	return;
}





