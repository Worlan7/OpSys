/*  philosophers - Demonstrates the semaphore solution to the dining
*   philosophers problem. 
*
*  	Name : Elom Kwame Worlanyo
* 	Email : elomworlanyo@wustl.edu 
*/


#ifndef UTIlITY_H
#define UTIlITY_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <limits>
#include <vector>

/* Global variables */
/* End global variables */


/* Function prototypes */
void initialize(int argc, char **argv, int &num_philosophers, int &drinking, std::vector<std::vector<int>> &BottleLocations);
void read_matrix(std::string file, int &num_philosophers, std::vector<std::vector<int>> &BottleLocations);






#endif
