# Elom Worlanyo, ID: 419493 - Operating Systems Project 1 
# Tutor: Professor Lawrence Miller 

xssh: xssh.c utility.c xssh.h utility.h
	gcc -ansi -g -I -Wall xssh.c utility.c -o xssh -lc -lreadline -lcurses
