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

#include "utility.h"
#include "philosophers.h"


int main(int argc, char **argv) 
{	
	initialize(argc, argv, num_philosophers, drinking, BottleLocations); 
	int i, targ[num_philosophers];
	std::string philosophers_a = "";
	std::string philosophers_b = "";
	pthread_t thread[num_philosophers];
	sem_t newForks[num_philosophers];
	Fork = newForks;

	for(i = 0; i < num_philosophers; i++){
		//array(vector) showing philosopher state
		activity.push_back(0);
	}

	if(drinking == 0)
	{
  		std::cout << "You chose dining philosophers" << std::endl;
  		std::cout << "There are "  << num_philosophers 
  					<< " philosophers"<<std::endl;

        for(i=0;i<num_philosophers;i++)
        {
            int print = i%10;
            if(i > 9){
                philosophers_a += "1";
            }else{
                philosophers_a += " ";
            }
            philosophers_b += std::to_string(print);
        }
        
        printf("%s\n%s\n", philosophers_a.c_str(), philosophers_b.c_str());
            
	    sem_init(&access_activity, 0, num_philosophers - 1);    
        
	    for(i=0;i<num_philosophers;i++) 
	    {
	    	if(sem_init(&Fork[i], 0, 1) == 0){
	    	}else{
	    		printf("could not init semaphore\n");
                exit(-1);
	    	} 
	    }
	    for(i=0;i<num_philosophers;i++) 
	    {
	        targ[i] = i;
	        pthread_create(&thread[i], NULL, &dining_philosopher,(void *) &targ[i]);
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
  		std::vector<std::vector<sem_t>> bottles(num_philosophers, std::vector<sem_t>(num_philosophers));
  		Bottle = bottles;

  		sem_init(&screen, 0, num_philosophers - 1);    
  		for(i=0;i<num_philosophers;i++) 
	    {
	    	for(int j = 0; j < num_philosophers; j++){
	    		if(BottleLocations[i][j] == 1){
		    		if(sem_init(&Bottle[i][j], 0, 1) == 0){
		    		}else{
		    			printf("could not init semaphore\n");
	                	exit(-1);
		    		}
		    	}
	    	} 
	    }

	    for(i=0;i<num_philosophers;i++) 
	    {
	        targ[i] = i;
	        pthread_create(&thread[i], NULL, &drinking_philosopher,(void *) &targ[i]);
	    }
	    for(i=0;i<num_philosophers;i++) 
	    {
	        pthread_join(thread[i], NULL);
	    }
	    for(i=0;i<num_philosophers;i++) 
	    {
	    	for(int j = 0; j < num_philosophers; j++){
	    		if(BottleLocations[i][j] == 1){
		    		sem_destroy(&Bottle[i][j]);
		    	}
	    	} 
	    }

	    sem_destroy(&screen);


  	}

    
    return 0;
}





void *dining_philosopher(void *ptr) 
{
    int i, k = *((int *) ptr);

    for(i = 1; i <= eat_limit; i++) 
    {
    	unsigned int loc = k;
    	int a = (k+1) % num_philosophers;
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


void *drinking_philosopher(void *ptr)
{
    int i, k = *((int *) ptr);
    std::vector<std::pair<int, int>> available_drinks;

    /* Get the drinks that a given philosopher may drink from*/
    for(i=0;i<num_philosophers;i++) 
    {
    	for(int j = 0; j < num_philosophers; j++)
    	{
    		if((i == k || j == k) && (BottleLocations[i][j] == 1))
    		{
    			std::pair<int, int> bottle = std::make_pair(i, j);
    			available_drinks.push_back(bottle);
    		}
    	} 
    }

    for(size_t i = 0; i < available_drinks.size(); i++)
    {
    	printf("%d %d\n", available_drinks[i].first, available_drinks[i].second);
    }

    printf("this is philosopher %d \n", k);
    for(i = 1; i <= eat_limit; i++) 
    {
    	unsigned int loc = k;
        think(loc); 
        sem_wait(&screen);
        drink(loc, available_drinks);
        sem_post(&screen);
    }
    pthread_exit(0);

}


void eat(unsigned &k)
{
	activity[k] = 1;
	dining_print();
	sleep(rand() % 5 + 5);
    activity[k] = 0;
    dining_print();

}

void drink(unsigned &k, std::vector<std::pair<int, int>> available_drinks)
{
	int num_drinks = rand() % 1 + static_cast<int>(available_drinks.size());
	// obtain a time-based seed:
  	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
 	std::shuffle (available_drinks.begin(), available_drinks.end(), std::default_random_engine(seed));
 	for(int i = 0; i < num_drinks; i++){
 		std::cout << available_drinks[i].first << available_drinks[i].last << std::endl;
 	}

}


void dining_print()
{
	std::string activities;
	for(int i = 0; i < num_philosophers; i++){
        unsigned p = i;
        if(activity[p] == 1){
            activities += "*";
        }
        else
        {
            activities += " ";
        }

	}
	printf("%s\n", activities.c_str());
}

void think (unsigned &k)
{	
	sleep(rand() % 20 + 5);
}

