/*  philosophers.cpp - Demonstrates the semaphore solution to the dining
*   philosophers problem. The activity_access semaphore is used to prevent all
*   philosophers from entering the access_activity and grabbing their left most fork
*   at the same time, which would lead to deadlock. This program also 
*   demonstrates a solution to the drinking philosophers problem, with input
*   consisting of a file, within which a matrix is used to represent the 
*   graph of shared resources.
*
*  	Name : Elom Kwame Worlanyo
* 	Email : elomworlanyo@wustl.edu 
*/

#include "philosophers.h"
#include "utility.h"


int main(int argc, char **argv) 
{
	initialize(argc, argv, num_philosophers, drinking); 
	int i, targ[num_philosophers];
	pthread_t thread[num_philosophers];

	for(i = 0; i < num_philosophers; i++){
		//array(vector) showing philosopher state
		activity.push_back(0);
	}

	if(drinking == 0)
	{
  		std::cout << "You chose dining philosophers" << std::endl;
  		std::cout << "There are "  << num_philosophers 
  					<< " philosophers"<<std::endl;

	    sem_init(&access_activity, 0, num_philosophers - 1);    

	    for(i=0;i<num_philosophers;i++) 
	    {
	    	if(sem_init(&Fork[i], 0, 1) == 0){
	    		printf("fork init"); 
	    	}else{
	    		printf("could not init semaphore");
	    	}    
	    }
	    for(i=0;i<num_philosophers;i++) 
	    {
	        targ[i] = i;
	        pthread_create(&thread[i], NULL, &tphilosopher,(void *) &targ[i]);
	    }
	    for(i=0;i<num_philosophers;i++) 
	    {
	        pthread_join(thread[i], NULL);
	    }
	    for(i=0;i<num_philosophers;i++) 
	    {
	        sem_destroy(&Fork[i]);
	    }
	    sem_destroy(&access_activity);

  	}
  	else
  	{
  		std::cout << "You chose drinking philosophers" << std::endl;
  		std::cout << "There are "  << num_philosophers<< std::endl;
  	}

   
    
    return 0;
}





void *tphilosopher(void *ptr) 
{
    int i, k = *((int *) ptr);

    for(i = 1; i <= eat_limit; i++) 
    {
    	unsigned int loc = k;
    	int a = (k+1) % num_philosophers;
        //printf("%*cThink %d %d\n", k*4, ' ', k, i);
        think(loc);
        sem_wait(&access_activity);
        if(a < k){
        	sem_wait(&Fork[a]);
        	sem_wait(&Fork[k]);
    	}
    	else
    	{
    		sem_wait(&Fork[k]);
        	sem_wait(&Fork[a]);
    	}
    	
    	eat(loc);

    	sem_post(&Fork[a]);
        sem_post(&Fork[k]);
        sem_post(&access_activity);
    }
    pthread_exit(0);
}



void eat(unsigned &k)
{
	activity[k] = 1;
	std::string activities = "";
	for(unsigned p = 0; p < num_philosophers; p++){
		activities += " " + std::to_string(activity[p]) + " ";

		/*if(activity[p] == 1){
			printf(" * ");
		}
		else
		{
			printf("  ");
		}	
		*/
	}
	printf("%s\n", activities.c_str());
	sleep(rand() % 5 + 5);

}


void think (unsigned &k)
{
	activity[k] = 0;
	std::string activities;
	for(unsigned p = 0; p < num_philosophers; p++){
		activities += " " + std::to_string(activity[p]) + " ";
		/*
		if(activity[p] == 1){
			printf(" * ");
		}
		else
		{
			printf("  ");
		}
		*/
	}
	printf("%s\n", activities.c_str());
	sleep(rand() % 20 + 5);
}

