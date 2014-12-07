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

#ifndef UTIlITY_H
#define UTIlITY_H

#include <string>
#include <iostream>
#include <sstream>
#include <random>
#include <fstream>
#include <vector>
#include <unistd.h>


/* Helper Functions*/
void usage(void);
bool legal_int(char *str);
std::vector<int> read_input(std::string input_file);
void print_references(int page, std::vector<int> references, bool fault);
int furthest_away(size_t index, std::vector<int> physical, std::vector<int> mem_references);

#endif