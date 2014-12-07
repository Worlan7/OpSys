/*
*
* vmgenU -  a simple page reference generator. Generates a sequence of page 
* references of the desired length 
*
* Name : Elom Kwame Worlanyo
* Email : elomworlanyo@wustl.edu 
*
*/

#include "vmgenU.h"


int main(int argc, char **argv) 
{
	int range_from = 0;
	int range_to, length;
	std::string output_file;
	std::ofstream vmrefs;

	//if wrong number of arguments
	if(argc != 4)
	{
		usage();
	}
	else
	{
		//Value checking
		if(!legal_int(argv[1]) || !legal_int(argv[2]))
		{
			std::cout << "One or more arguments are not integers" << std::endl;
			usage();
		}

		//Parse command-line arguments
		range_to = atoi(argv[1]);
		length = atoi(argv[2]);
		output_file = argv[3];

		if(range_to < 1 || range_to > 100)
		{
			usage();
		}
		else
		{
			//Open file
			vmrefs.open(output_file);
			//Uniform number generation
			std::random_device rand_dev;
    		std::mt19937 generator(rand_dev());
    		std::uniform_int_distribution<int>  distr(range_from, (range_to-1));

    		if(vmrefs.is_open())
    		{
	    		for(int i = 0; i < length; ++i)
	    		{
	 		       vmrefs << distr(generator) << " ";
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
    std::cout<< "Usage: vmgenU| range| length| output_file" << std::endl;
    std::cout<< "       range  -> page numbers from 0 - (range-1)" << std::endl;
    std::cout<< "  		range input must be pos num less than 100" << std::endl;
    std::cout<< "       length  -> number of page numbers" << std::endl;
    std::cout<< "       output_file -> name of output file" << std::endl;
    exit(1);
}