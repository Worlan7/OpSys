/*
*
* vmgenWS -  a simple page reference generator. Generates a sequence of page 
* references of the desired length 
*
* Name : Elom Kwame Worlanyo
* Email : elomworlanyo@wustl.edu 
*
*/


#include "vmgenWS.h"


int main(int argc, char **argv) 
{
	std::vector<int> page_references;
	int range_from = 0;
	int ws_size, num_gen, range_to, length, lower_bound, upper_bound;
	std::string output_file;
	std::ofstream vmrefs;

	//if wrong number of arguments
	if(argc != 7)
	{
		usage();
	}else
	{
		//Value checking 
		for (int i = 1; i < 5; ++i)
		{
			if(!legal_int(argv[i]))
			{
				std::cout << "param " << i << " should be an int" << std::endl;
				usage();
			}

		}

		//Parse command-line arguments
		ws_size = atoi(argv[1]);
		lower_bound = atoi(argv[2]);
		upper_bound = atoi(argv[3]);
		range_to = atoi(argv[4]);
		length = atoi(argv[5]);
		output_file = argv[6];

		//Format checking
		if(upper_bound < lower_bound)
		{
			std::cout << "upper bound less than lower bound" << std::endl;
			usage();
		}


		if(range_to < 1 || range_to > 100)
		{
			usage();
		}
		else
		{
			//Uniform number generation
			std::random_device rand_dev;
    		std::mt19937 generator(rand_dev());
    		std::uniform_int_distribution<int>  distr(range_from, range_to);

    		while(page_references.size() < length)
    		{
    			std::vector<int> working_set;
	    		num_gen = rand() % (upper_bound - lower_bound + 1) + lower_bound;

	    		for(int i = 0; i < ws_size; ++i)
	    		{
	    			working_set.push_back(distr(generator));
	    		}

	    		for(int i = 0; i < num_gen; ++i)
	    		{
	    			int loc = rand() % ws_size;
	    			if(page_references.size() < length)
	    			{
	    				page_references.push_back(working_set[loc]);
	    			}else{
	    				break;
	    			}
	    		}

 			}

 			//Open file
			vmrefs.open(output_file);
    		if(vmrefs.is_open())
    		{
	    		for(size_t i = 0; i < page_references.size(); ++i)
	    		{
	 		       vmrefs << page_references[i] << " ";
	 			}
	 			vmrefs << std::endl;
	 			vmrefs.close();
 			}
 			else
 			{
 				std::cout << "Unable to open file" << std::endl;
 				exit(-1);
 			}
		}
	}

	return 0;
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
 * usage - print a usage message
 */
void usage(void) 
{
    std::cout<< "Usage: vmgenWS| ws_size| lower_bound| upper_bound| " <<
    			"range| length| output_file "<< std::endl;
    std::cout<< "       ws_size -> the size of the working set"<< std::endl;
    std::cout<< "       lower_bound -> lower bound on num of pages"<< std::endl;
    std::cout<< "       upper_bound -> upper bound on num of pages"<< std::endl;
    std::cout<< "       range  -> page numbers from 0 - range" << std::endl;
    std::cout<< "  		range input must be pos num less than 100" << std::endl;
    std::cout<< "       length  -> number of page numbers" << std::endl;
    std::cout<< "       output_file -> name of output file" << std::endl;
    exit(1);
}