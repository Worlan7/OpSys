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
	//Get user input to determine program function
	initialize(argc, argv, num_philosophers, drinking, BottleLocations); 
	//used to identify philosophers
	int i, targ[num_philosophers];
	std::string philosophers_a = "";
	std::string philosophers_b = "";

	if(drinking == 0)
	{
		//thread and semaphores for dining philosophers
		pthread_t thread[num_philosophers];
		sem_t newForks[num_philosophers];
		Fork = newForks;

		for(i = 0; i < num_philosophers; i++){
			//array(vector) showing philosopher state
			activity.push_back(0);
		}

  		std::cout << "You chose dining philosophers" << std::endl;
  		std::cout << "There are "  << num_philosophers 
  					<< " philosophers"<<std::endl;

  		//Print out to identify philosophers by column
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
            
        //initialising access_activity semaphore. Functions similar to Room
        //semaphore discussed in class. If error exit.
	    if(sem_init(&access_activity, 0, num_philosophers - 1) != 0){
	    	printf("could not init access_activity semaphore\n");
	    	exit(-1);
	    }    
        //Initialising semaphores on forks. If error exit
	    for(i=0;i<num_philosophers;i++) 
	    {
	    	if(sem_init(&Fork[i], 0, 1) != 0){
	    		printf("could not init a Fork semaphore\n");
                exit(-1);
	    	}
	    }
	    //Spawning off threads
	    for(i=0;i<num_philosophers;i++) 
	    {
	        targ[i] = i;
	        pthread_create(&thread[i], NULL, &dining_philosopher,(void *) &targ[i]);
	    }
	    //Making main process wait on children threads
	    for(i=0;i<num_philosophers;i++) 
	    {
	        pthread_join(thread[i], NULL);
	    }
	    //Destroying semaphores once done
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
  		//2d vector mapping bottles to semaphores. Point global to initialized location
  		std::vector<std::vector<sem_t>> bottles(num_philosophers, std::vector<sem_t>(num_philosophers));
  		Bottle = bottles;

  		//Semaphore controlling screen access
  		sem_init(&screen, 0, num_philosophers - 1);   
  		//Initializing bottle semaphore if indicated by bottle location 
  		for(i=0;i<num_philosophers;i++) 
	    {
	    	for(int j = 0; j < num_philosophers; j++){
	    		if(BottleLocations[i][j] == 1){
		    		if(sem_init(&Bottle[i][j], 0, 1) != 0){
		    			printf("could not init semaphore\n");
	                	exit(-1);
		    		}
		    	}
	    	} 
	    }

	    /*
	    * This block of code functions almost identically to dining philosophers above.
	    */
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

/*
*	Function called by a dining philosopher thread. This function allows a 
*	philosopher to eat without deadlocks or starvation, by preventing circular
*	wait. Use of access_activity to prevent further race conditions.
*
*/

void *dining_philosopher(void *ptr) 
{
	//int k identifies philosopher thread by number
    int i, k = *((int *) ptr);

    //eat limit is number of times philosopher eats before program ends
    for(i = 1; i <= eat_limit; i++) 
    {
    	unsigned int loc = k;
    	int a = (k+1) % num_philosophers;
    	//calling think function, which functionally pauses the thread
        think(loc); 
        sem_wait(&access_activity);
        //making wait order different prevents circular wait.
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

/*
*	Function called by a drinking philosopher thread. This function allows a 
*	philosopher to drink without deadlocks or starvation.
*
*/

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

    //eat_limit is functionally drink limit, number of times philosopher drinks
    for(i = 1; i <= eat_limit; i++) 
    {
    	unsigned int loc = k;
        think(loc); 
        sem_wait(&screen);
        //account for possibility that philosopher has no drinks available
        if(available_drinks.size() != 0)
        	drink(loc, available_drinks);

        sem_post(&screen);
    }
    pthread_exit(0);
}


/*
*
*	Function to simulate eating. Sleeps between 5 and 10 seconds and prints out
*	status
*/
void eat(unsigned &k)
{
	activity[k] = 1;
	dining_print();
	sleep(rand() % 5 + 5);
    activity[k] = 0;
    dining_print();

}


/*
*
*	Function to simulate drinking. Sleeps between 5 and 10 seconds and prints out
*	status. This function uses std::shuffle to obtain a random group of drinks
*	per session for any given philosopher.
*/

void drink(unsigned &k, std::vector<std::pair<int, int>> available_drinks)
{
	std::vector<std::pair<int, int>> desired_drinks;	//drinks needed for session
	int size = static_cast<int>(available_drinks.size());	//number of drinks available to philosopher
	int num_drinks = rand() % size + 1;		//number of drinks philosopher needs per session

	//Strings used to form output
	std::string append = std::string("Philosopher ") + std::to_string(k) + ":" ; 
	std::string drinks_used = std::string("bottles ");

	//obtain a time-based seed:
  	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
 	std::shuffle (available_drinks.begin(), available_drinks.end(), std::default_random_engine(seed));

 	//Form textual representation of drinks required by philosopher
 	for(int i = 0; i < num_drinks; i++){
 		desired_drinks.push_back(available_drinks[i]);
 		std::string bottle_string = std::string("(") + std::to_string(available_drinks[i].first) + ", " +
 									std::to_string(available_drinks[i].second) + ")";
 		drinks_used += bottle_string;
 	}

 	//sort required drinks to ensure philosopher waits from lowest up.
 	std::sort(desired_drinks.begin(), desired_drinks.end());

 	//waiting on relevant semaphores
 	for(int i = 0; i < num_drinks; i++){
 		sem_wait(&Bottle[desired_drinks[i].first][desired_drinks[i].second]);
 	}
 	//print prompt
 	printf("%s drinking from %s \n", append.c_str(), drinks_used.c_str());
 	//drink simulation
 	sleep(rand() % 5 + 5);
 	//drop semaphores
 	for(int i = 0; i < num_drinks; i++){
 		sem_post(&Bottle[desired_drinks[i].first][desired_drinks[i].second]);
 	}
 	//update on status
 	printf("%s putting down %s \n", append.c_str(), drinks_used.c_str());
}

/*
*
*	Helper function to textually represent state of philosophers
*/

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

/*
* Helper function to simulate thinking. Thinks between 5 and 25 seconds
*/
void think (unsigned &k)
{	
	sleep(rand() % 20 + 5);
}

