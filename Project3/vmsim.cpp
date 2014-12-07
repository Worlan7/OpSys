/*
*
* vmsim -  a simple program that simulates virtual memory page replacement
* algorithms 
*
* Name : Elom Kwame Worlanyo
* Email : elomworlanyo@wustl.edu 
*
*/

#include "vmsim.h"
#include "utility.h"


int main(int argc, char **argv) 
{
	init = -1;	//Empty frames represented with int -1
	num_references= 0;
	num_faults = 0;
	parse_cmd_line(argc, argv);
	memory_references = read_input(input_file);

	//Initializing physical memory frames
	for(int i = 0; i < num_frames; ++i)
	{
		physical.push_back(init);
	}

	switch(alg_flag)
	{
		case OPT:
			run_optimum();
			break;
		case LRU:
			run_lru();
			break;
		case FIFO:
			run_fifo();
			break;
		case CLOCK:
			run_clock();
			break;
	}

	return 0;
}


/*
* parse_cmd_line - extract command line arguments and check for correctness
*/
void parse_cmd_line(int argc, char **argv)
{
	if(argc != 4)
	{
		usage();
	}
	else
	{
		//Value checking
		if(!legal_int(argv[1]))
		{
			usage();
		}
		
		num_frames = atoi(argv[1]);
		input_file = argv[2];

		//Get what kind of algorithm is being used.
		alg_flag = hash_algorithm(argv[3]);
		//Make sure num_frames within acceptable range
		if(num_frames < 1 || num_frames > 100)
		{
			usage();
		}
	}
}


/*
* hash_algorithm - simple helper function to convert char * to algorithm enum.
*
*/
algorithm hash_algorithm(char *alg)
{
	std::string alg_string = alg;

	if(alg_string == "OPT" || alg_string == "opt")
		return OPT;
	if(alg_string == "LRU" || alg_string == "lru")
		return LRU;
	if(alg_string == "FIFO" || alg_string == "fifo")
		return FIFO;
	if(alg_string == "CLOCK" || alg_string == "clock")
		return CLOCK;

	//No matches
	usage();
	//Will not reach
	exit(-1);
} 


/*
* run_optimum - runs the optimum algorithm for page replacement
*
*/
void run_optimum(void)
{
	bool fault;
	std::vector<int>::iterator find;

	for(size_t loc = 0; loc < memory_references.size(); ++loc)
	{
		//check if memory_reference is already present in physical table
		find = std::find(physical.begin(), physical.end(), memory_references[loc]);
		//memory reference already in physical table.
		if(find != physical.end())
		{
			//No fault because already found
			fault = false;
		}
		else
		{
			//check if empty location present
			find = std::find(physical.begin(), physical.end(), init);
			//empty location present
			if(find != physical.end())
			{
				//place in empty location
				*find = memory_references[loc];
				//No fault. Empty location was available.
				fault = false;
			}
			else
			{
				//NO empty location available. Fault has occured
				fault = true;
				num_faults++;
				
				int furthest = furthest_away(loc, physical, memory_references);
				find = std::find(physical.begin(), physical.end(), furthest);

				//Problem has happened, abort.
				if(find == physical.end())
				{
					std::cout << "Problem with furthest iterator. Exiting" << std::endl;
					exit(-1);
				}

				//Page fault. Replacing memory location that is furthest away.
				*find = memory_references[loc];
			}
		}

		if(loc >= num_frames)
		{
			num_references++;	//Should equal number of memory_references - num_frames
		}
		print_references(memory_references[loc], physical, fault);
		//usleep(100000);
	}
	std::cout << "optimum:" << " num faults " << num_faults << " num_references " << num_references<< std::endl;
}

/*
* run_lru- runs the lru algorithm for page replacement
*
*/
void run_lru(void)
{
	bool fault;
	std::vector<int> lru_indices;	
	for(int i = 0; i < num_frames; ++i)
	{
		lru_indices.push_back(init);
	}
	std::vector<int>::iterator find;

	for(size_t loc = 0; loc < memory_references.size(); ++loc)
	{
		//check if memory_reference is already present in physical table
		find = std::find(physical.begin(), physical.end(), memory_references[loc]);
		//memory reference already in physical table.
		if(find != physical.end())
		{
			//No fault because already found
			fault = false;
			//Place index in lru_indices
			int phys_loc = find - physical.begin();
			lru_indices[phys_loc] = loc;
		}
		else
		{
			//check if empty location present
			find = std::find(physical.begin(), physical.end(), init);
			//empty location present
			if(find != physical.end())
			{
				//place in empty location
				*find = memory_references[loc];
				//Place index in lru_indices
				int phys_loc = find - physical.begin();
				lru_indices[phys_loc] = loc;
				//No fault. Empty location was available.
				fault = false;
			}
			else
			{
				//NO empty location available. Fault has occured
				fault = true;
				num_faults++;
				
				//least recently used has smallest index
				int lru_index = std::distance(lru_indices.begin(), min_element(lru_indices.begin(), lru_indices.end()));
				//Page fault. Replacing memory location that is least recently used.
				physical[lru_index] = memory_references[loc];
				//Place index in lru_indices
				lru_indices[lru_index] = loc;
			}
		}

		if(loc >= num_frames)
		{
			num_references++;	//Should equal number of memory_references - num_frames
		}
		print_references(memory_references[loc], physical, fault);
		//usleep(100000);
	}
	std::cout << "lru:" << " num faults " << num_faults << " num_references " << num_references<< std::endl;
}


/*
* run_fifo - runs the fifo algorithm for page replacement
*
*/
void run_fifo(void)
{
	bool fault;
	std::vector<int> indices;	
	for(int i = 0; i < num_frames; ++i)
	{
		indices.push_back(init);
	}
	std::vector<int>::iterator find;

	for(size_t loc = 0; loc < memory_references.size(); ++loc)
	{
		//check if memory_reference is already present in physical table
		find = std::find(physical.begin(), physical.end(), memory_references[loc]);
		//memory reference already in physical table.
		if(find != physical.end())
		{
			//No fault because already found
			fault = false;
		}
		else
		{
			//check if empty location present
			find = std::find(physical.begin(), physical.end(), init);
			//empty location present
			if(find != physical.end())
			{
				//place in empty location
				*find = memory_references[loc];
				//Place index in indices
				int phys_loc = find - physical.begin();
				indices[phys_loc] = loc;
				//No fault. Empty location was available.
				fault = false;
			}
			else
			{
				//NO empty location available. Fault has occured
				fault = true;
				num_faults++;
				
				//least recently used has smallest index
				int index = std::distance(indices.begin(), min_element(indices.begin(), indices.end()));
				//Page fault. Replacing memory location that is least recently used.
				physical[index] = memory_references[loc];
				//Place index in indices
				indices[index] = loc;
			}
		}

		if(loc >= num_frames)
		{
			num_references++;	//Should equal number of memory_references - num_frames
		}
		print_references(memory_references[loc], physical, fault);
		//usleep(100000);
	}
	std::cout << "fifo:" << " num faults " << num_faults << " num_references " << num_references<< std::endl;
}


/*
* run_clock - runs the clock algorithm for page replacement
*
*/
void run_clock(void)
{
	std::cout << "clock" << std::endl;
}












