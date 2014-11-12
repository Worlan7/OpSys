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

    if(drinking != 1){
	    std::cout<< "Please enter number of philosophers. (2 - 15)" << std::endl;
	    std::cin >> num_philosophers;
	    std::cin.clear();

	    if(num_philosophers < 2 || num_philosophers > 15){
		    std::cout << "invalid number of philosophers" << std::endl;
		    exit(-1);
    	}
    }
    else
    {
    	std::string args;
   		std::cout<< "Please enter name of file containing philosopher matrix" << std::endl;
	    std::cin >> args;

    	std::ifstream ifs (args);
		//check if file can be opened
		if(ifs.is_open())
		{	
			/* Getting number of philosophers present in the file*/
			num_philosophers = 0;
			num_check = 0;
			int bottle = 0;
			std::string philosopher_line;
			while (std::getline(ifs, unused) ){
				std::string temp;
				std::istringstream philosopher_set(philosopher_line);
				bottle = num_philosophers;
				while(philosopher_set >> temp)
				{
					

				}
			   ++num_philosophers;
			}


    	}
    }

}


void read_matrix(char **file)
{

}
