/*  philosophers - Demonstrates the semaphore solution to the dining
*   philosophers problem. 
*
*  	Name : Elom Kwame Worlanyo
* 	Email : elomworlanyo@wustl.edu 
*/

#include "utility.h"

void initialize(int argc, char **argv, int &num_philosophers, int &drinking)
{
	std::cout<< "Enter 1 for drinking philosophers. Defaults to dining " 
	"on any other character" << std::endl;
	
	if(std::cin >> drinking)
	{
		if(drinking != 1)
		{
			drinking = 0;
		}
		std::cin.clear();
	}
	else
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		drinking = 0;
	}	

	std::cout<< "Please enter number of philosophers. (2 - 15)" << std::endl;
	std::cin >> num_philosophers;
	std::cin.clear();

	if(num_philosophers < 2 || num_philosophers > 15){
		std::cout << "invalid number of philosophers" << std::endl;
		exit(-1);
	}

}


void read_file(char **file)
{

}