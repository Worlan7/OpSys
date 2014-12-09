/*
*
* vmsim -  a simple program that simulates virtual memory page replacement
* algorithms.
* utility contains any helper functions necessary for this program
*
* Name : Elom Kwame Worlanyo
* Email : elomworlanyo@wustl.edu 
*
*/

#include "utility.h"


/*
 * usage - print a usage message
 */
void usage(void) 
{
    std::cout<< "Usage: vmsim| num_frames| input_file| algorithm" << std::endl;
    std::cout<< "       num_frames  -> (max 100)" << std::endl;
    std::cout<< "       total number of physical memory frames" << std::endl;
    std::cout<< "       input_file -> name of input file" << std::endl;
    std::cout<< "       algorithm  -> page replacement algorithm" << std::endl;
    std::cout<< "       'OPT', 'CLOCK', 'LRU', 'FIFO' case-insens" << std::endl;
    exit(1);
}


/*
* legal_int - check if input is integer
*/
bool legal_int(char *str)
{
	while(*str != 0)
  	{
		if(!isdigit(*str++))
		{
			return false;
		}
	}
	return true;
}

/*
* read_input - extracts a vector of memory references from file 
*/

std::vector<int> read_input(std::string input_file)
{
	std::vector<int> mem_references;
	int page;
	std::ifstream vmrefs;
	vmrefs.open(input_file);

	if(vmrefs.is_open())
	{		
		std::string references, temp;
		std::stringstream buffer;
		//pull entire contents of file
		buffer << vmrefs.rdbuf();
		std::getline(buffer, references);
		std::istringstream reference_stream(references);

		//push page references into vector
		while(reference_stream >> temp)
		{
			//Make sure all elements in input file are legal integers
			char *tempstr = &temp[0];
			if(!legal_int(tempstr)){
				std::cout << "Illegal char in input file" << std::endl;
				usage();
			}
			page = atoi(temp.c_str());

			if(page >= 100){
				std::cout << "Invalid int in input file" << std::endl;
				usage();
			}
			mem_references.push_back(page);
		}

	}
	else
	{
		std::cout << "Problem opening input file" << std::endl;
		usage();
	}

	return mem_references;
}


/*
* print_references - Prints out the state of the physical references in the 
*					 required format.
*/
void print_references(int page, std::vector<int> references, bool fault)
{
	std::string out;

	if(page < 10)
		std::cout << " " << page << ": ";
	else
		std::cout << page << ": ";

	for(size_t i = 0; i < references.size(); ++i )
	{	
		//Formatting
		if(references[i] < 10)
		{
			//Empty
			if(references[i] < 0){
				out = "  ";
			}
			else
			{
				out = " " + std::to_string(references[i]);
			}
		}
		else
		{
			out = std::to_string(references[i]);
		}

		if(i == 0)
		{
			std::cout << "[" << out;
		}
		else
		{
			std::cout << "|" << out; 
		}
	}

	if(fault)
	{
		std::cout << "]  F" << std::endl;
	}
	else
	{
		std::cout << "]" << std::endl;
	}
}

/*
*
* farthest_away: returns the integer that is furthest away from the current 
* index
*
*/

int furthest_away(size_t index, std::vector<int> physical, 
	std::vector<int> mem_references)
{
	int max_distance = 0;
	int current_furthest;

	for(size_t i = 0; i < physical.size(); ++i)
	{
		int current = physical[i];
		int distance = 0;

		for(size_t j = index; j < mem_references.size(); ++j)
		{

			if(current == mem_references[j])
			{
				break;
			}
			++distance;
		}

		if(distance > max_distance)
		{
			max_distance = distance;
			current_furthest = current;
		}
	}

	return current_furthest;
}











