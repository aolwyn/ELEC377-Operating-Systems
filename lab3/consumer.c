//+
//    ELEC 377,  Lab 3
//
//  consumer.c
//-

#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <errno.h>

#include "common.h"

#define FALSE 0
#define TRUE 1

//+
//Function: main 
//
//Purpose: contains the code to access the shared memory segment and map it into memory. 
//copies the data from the shared memory (from the producer) to the output one byte at a time.
//Only does the above if and only if it is allowed to (has the mutex lock) and if 
//there exists something in the input file to read to the output file. 
//
//Parameters: standard, auto-generated aguments (int argc, char*argv[]). 
//			  Utilizes methods and things from common header file, but are not parameters.
//
//Returns: 0 (nothing)
//-

int main (int argc, char *argv[]){

	// initialize the shared memory, load in the initial array's, spawn the worker
	// processes.
	char c;
	key_t   key;
	struct shared    *sharedPtr;
	int shmid, numProd;
	int charRead = FALSE;
    
	/*	 Shared memory init 	*/
	key = ftok(".", 'S');
	if((shmid = shmget(key, MEMSIZE, IPC_CREAT|0666)) == -1 ){
		if( (shmid = shmget(key, MEMSIZE, 0)) == -1){
			printf("Error allocating shared memory. \n");
			exit(1);
		}
	}

	// now map the region..
	if((int)(sharedPtr = (struct shared *) shmat(shmid, 0, 0)) == -1){
	    printf("Couldn't map the memory into our process space.\n");
	    exit(1);
	}
	
    // put your code here...
	getMutex(&sharedPtr->lock);
	numProd = sharedPtr->numProducers;
	releaseMutex(&sharedPtr->lock);

	charRead = TRUE;
	while(numProd > 0 && charRead){
		charRead = FALSE;
		while(charRead == FALSE && numProd > 0){
			getMutex(&sharedPtr->lock);
			if(sharedPtr->count > 0){
				c = sharedPtr->buffer[sharedPtr->out];
				sharedPtr->out = (sharedPtr->out + 1) % BUFFSIZE;
				sharedPtr->count--;
				charRead = TRUE;
			}else{
				numProd = sharedPtr->numProducers;
			}
			releaseMutex(&sharedPtr->lock);
		}
		putchar(c);
	}
    
	return 0;
}


