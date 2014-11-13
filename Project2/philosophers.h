/*  philosophers - Demonstrates the semaphore solution to the dining
*   philosophers problem. 
*
*  	Name : Elom Kwame Worlanyo
* 	Email : elomworlanyo@wustl.edu 
*/


#ifndef PHILOSOPHERS_H
#define PHILOSOPHERS_H

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>

/* Global variables */
int eat_limit = 5; /* Number of times each philosopher tries to eat */
int num_philosophers;
std::vector<int> activity;  		
int drinking;

sem_t access_activity;
sem_t *Fork;

std::vector<std::vector<int>> BottleLocations;
std::vector<std::vector<sem_t>>  Bottle;
/* End global variables */


/* Function prototypes */
void *tphilosopher(void *ptr);
void eat(unsigned &k);
void think (unsigned &k);
void dining_print();



#endif
