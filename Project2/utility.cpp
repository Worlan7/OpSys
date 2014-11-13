/*  philosophers - Demonstrates the semaphore solution to the dining
*   philosophers problem. 
*
*  	Name : Elom Kwame Worlanyo
* 	Email : elomworlanyo@wustl.edu 
*/

#include "utility.h"

void initialize(int argc, char **argv, int &num_philosophers, int &drinking, std::vector<std::vector<int>> &BottleLocations)
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
	    read_matrix(args, num_philosophers, BottleLocations);
    	
    }

}


void read_matrix(std::string file, int &num_philosophers, std::vector<std::vector<int>> &BottleLocations)
{
	std::ifstream ifs (file);
	//check if file can be opened
	if(ifs.is_open())
	{	
		num_philosophers = 0;
		int row = 0;
		int bottle_loc = 0;
		std::stringstream buffer, count_buf;
		std::string philosopher_line, temp;

		//pull entire contents of file
		buffer << ifs.rdbuf();
		std::getline(buffer, philosopher_line);
		std::istringstream count_stream(philosopher_line);
		/* Getting number of philosophers present in the file*/
		while(count_stream >> temp){
			num_philosophers++;
		}

		//refreshing ifs
		ifs.clear();
		ifs.seekg(0, std::ios::beg);

		/*Putting bottles (and semaphores) in 2d array*/
		std::vector<std::vector<int>> bottle_locations(num_philosophers, std::vector<int>(num_philosophers));
		BottleLocations = bottle_locations;

		while (std::getline(ifs, philosopher_line) ){
			int col = 0;
			std::istringstream philosopher_set(philosopher_line);
			bottle_loc = row;
			while(philosopher_set >> temp)
			{
				if(temp == "1"){
					BottleLocations[row][bottle_loc + col] = 1;
				}
				col ++;
			}
				row++;
		}


		if(row == num_philosophers){ //num across = num vertical
			printf("copacetic\n");
			for(int i = 0; i < num_philosophers; i++){
				for(int j = 0; j < num_philosophers; j++){
					if((i == j) && BottleLocations[i][j] != 0){
						printf("Philosopher cannot share bottle with himself");
						exit(-1);
					}
				}
			}
		}else{
			printf("Must be symmetric square matrix");
			exit(-1);
		}
    }
}




