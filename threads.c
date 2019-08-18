/*	** Copyright Jack Belmont **
 * 
 * jOS Source Code
 * 
 * Source code for multithreading
 * 
*/

#include "threads.h"


/* Thread Pool (tcb_t) */
static tcb_t thread1;
static tcb_t thread2;
static tcb_t thread3;
static tcb_t* threadPool[] = {&thread1, &thread2, &thread3};


/* Queue Information */
static tcb_t* queueHead;
static tcb_t* queueTail;
static unsigned int queueSize = 0;
tcb_t* runningThread = NULL;


/* Kernel Thread Information */
uint32_t* bootThreadStackPtr;
static bool inBootThread = TRUE;


/* Yeild to next thread in queue */
void yeild(void) {
	scheduler();
	return;
}


/* Used to return out of a thread when it is finished with its task */
void threadReturn(void) {
	runningThread->assigned = FALSE;
	scheduler();
}

/* 
 * Create new thread and puts it in ready queue
 * Returns the id of the thread created
*/
int threadCreate(void* stack, void* func) {
	int nextThread;
	int i;
	
	/* Determine next thread id available */
	nextThread = -1;
	for (i = 0; i < MAX_THREADS; i++) {
		if (!threadPool[i]->assigned) {
			nextThread = i;
			break;
		}
	}
	
	if (nextThread == -1) {
		/* No thread available to assign */
		return -1;
	}
	
	/* Set up thread control block (tcb) for the new thread */
	tcb_t* thread = threadPool[nextThread];
	thread->id = nextThread;
	//thread->fdt = fdtPool[nextThread];
	thread->stackBase = (uint32_t*)(stack + STACKSIZE - 1);
	thread->task = func;
	thread->assigned = TRUE;
	
	/* 
	 * Set a fake context in the thread's stack so when we context
	 * switch to it, it apears that we are re-entering the thread
	 * rather than calling a new thread.
	*/
	*((thread->stackBase) - 0) = 0;										/* Arbitrary base of the stack */
	*((thread->stackBase) - 1) = (uint32_t)(func);						/* EIP */
	*((thread->stackBase) - 2) = 0x202;									/* FLAGS */
	*((thread->stackBase) - 3) = 0;										/* EAX */
	*((thread->stackBase) - 4) = 0;										/* ECX */
	*((thread->stackBase) - 5) = 0;										/* EDX */
	*((thread->stackBase) - 6) = 0;										/* EBX */
	*((thread->stackBase) - 7) = (uint32_t)(thread->stackBase - 1); 	/* ESP */
	*((thread->stackBase) - 8) = (uint32_t)(thread->stackBase);			/* EBP */
	/*is it ok for ESI and EDI to default to zero? */
	*((thread->stackBase) - 9) = 0;										/* ESI */
	*((thread->stackBase) - 10) = 0;									/* EDI */
	
	/* 
	 * Set the stack pointer to the "top" of the stack so we can easily return into
	 * it and immediatly pop the new context
	*/
	thread->stackPtr = (thread->stackBase) - 10;
	
	/* Put new thread into the ready queue */
	addThread(thread);
	
	return 0;
}


/* Removes a tcb_t from the queue and returns a pointer to it */
tcb_t* extractThread() {
	tcb_t* extract = queueHead;
	
	if (queueSize == 1) {
		/* Pulling out the last thread */
		queueHead = NULL;
		queueTail = NULL;
	} else {
		queueTail->next = queueHead->next;
		queueHead->next->prev = queueTail;
		queueHead = queueHead->next;
	}
	
	queueSize--;
	
	return extract;
}


/* Puts a thread into the ready queue */
void addThread(tcb_t* thread) {
	if (queueSize == 0) {
		queueHead = thread;
		thread->next = thread;
		thread->prev = thread;
		queueTail = thread;
	} else {
		queueTail->next = thread;
		thread->prev = queueTail;
		thread->next = queueHead;
		queueTail = thread;
	}
	
	queueSize++;
	
	return;
}


/* Determines which thread should be run next and switches to it */
void scheduler(void) {
	tcb_t* yieldedThread;
	
	__asm__ __volatile__ ("cli");
	
	if (inBootThread && queueSize > 0) {
		/* Switching out of initial thread to first task */
		inBootThread = FALSE;
		runningThread = extractThread();
		contextSwitchThreads(&bootThreadStackPtr, runningThread->stackPtr);
		
	} else if (inBootThread && queueSize <= 0) {
		/* No threads to schedule */
		perror("NOTHING TO SCHEDULE\n");
		__asm__ __volatile__ ("sti");
		
		return;
		
	} else {
		/* Context switch between two running threads */
		if (runningThread->assigned) {
			/* Thread still has work to do. Put it back in the queue */
			addThread(runningThread);
			
		} else if (queueSize <= 0) {
			/* No more threads to run -> Exit out of scheduler */
			contextSwitchThreads(&(runningThread->stackPtr), bootThreadStackPtr);
		}
		
		yieldedThread = runningThread;
		runningThread = extractThread();
		if (yieldedThread != runningThread) {
			/* If we need to switch to a new thread, do so now */
			contextSwitchThreads(&(yieldedThread->stackPtr), runningThread->stackPtr);
		}	
	}
	
	__asm__ __volatile__ ("sti");
	
	return;
}


/* Sets interrupts to start the scheduling proccess */
void startScheduler(void) {
	__asm__ __volatile__ ("sti");
	return;
}






