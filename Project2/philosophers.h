/*  philosophers - Demonstrates the semaphore solution to the dining
*   philosophers problem. 
*
*  	Name : Elom Kwame Worlanyo
* 	Email : elomworlanyo@wustl.edu 
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>

#ifndef PHILOSOPHERS_H
#define PHILOSOPHERS_H

/* Global variables */
int eat_limit = 5; /* Number of times each philosopher tries to eat */
int num_philosophers;
std::vector<int> activity;  		
int drinking;
sem_t access_activity;
sem_t *Fork;
/* End global variables */


/* Function prototypes */
void *tphilosopher(void *ptr);
void eat(unsigned &k);
void think (unsigned &k);





#endif