/*
*
* vmsim -  a simple program that simulates virtual memory page replacement
* algorithms 
*
* Name : Elom Kwame Worlanyo
* Email : elomworlanyo@wustl.edu 
*
*/

#ifndef VMSIM_H
#define VMSIM_H


#include <string>
#include <vector>
#include <iterator>
#include <algorithm>

/* Global Variables*/
int num_frames;		//number of physical memory frames
int init;
std::string input_file;	//input file name
std::vector<int> memory_references;	//memory references from input file
std::vector<int> physical;	//physical memory frames
int num_faults;		//total number of page faults
int num_references;	//total number of page references


enum algorithm		//show which algorithm is in use.
{
	OPT = 0,
	LRU = 1,
	FIFO = 2,
	CLOCK= 3
};
algorithm alg_flag;		

/* Helper Functions */
void parse_cmd_line(int argc, char **argv);
algorithm hash_algorithm(char *alg);
void run_optimum(void);
void run_lru(void);
void run_fifo(void);
void run_clock(void);


#endif