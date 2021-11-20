//+ 	ELEC 377, Lab 3
//
//   common.c contains routines to be used from both the
//   producer, and the  consumer
//   Mutual Exclusion routines will be here
//-

#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <asm/system.h>


#include "common.h"

#define FALSE 0
#define TRUE 1

//+
//Function: test_and_set
//
//Purpose: Swaps the new value for the value in the address given by the * lock. Checks if the value retrieved is the test value. 
//
//Parameters: Lock pointer, holds open/close value of the lock (1/0).
//
//Returns: Value calculated by __cmpxchg, the value in the address given by * lock.
//-

int test_and_set(int * lock){
    return __cmpxchg(lock,0,1,4);
}

//+
//Function: getMutex
//
//Purpose: Used to obtain the lock (mutex). While something else has the lock, it will wait in a loop until it is it's turn to possess the lock.
//
//Parameters: Lock pointer, holds open/close value of the lock (1/0).
//
//Returns: Nothing (void)
//-

void getMutex(int *  lock){
	// this should not return until it has mutual exclusion. Note that many versions of 
	// this will probobly be running at the same time.
	while (test_and_set(lock)==1);
}

//+
//Function: releaseMutex
//
//Purpose: To release the mutex. Releases the lock, setting it back to 0 (unlocked.) so that another part of the program may claim the lock.
//
//Parameters: lock pointer, holds open/close value of the lock (1/0).
//
//Returns: Nothing (void)
//-

void releaseMutex(int * lock){
	// set the mutex back to initial state so that somebody else can claim it
	*lock = 0;
}

