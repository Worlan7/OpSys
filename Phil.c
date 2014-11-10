/*
 * $Smake: cc -O -o %F %f msecond.o random_int.o -lpthread -lm
 * 
 * A solution to the dining philosophers problem, using pthreads, semaphores
 * and shared memory.
 *
 * Written 02/22/1998 by J. Senning based on code provided by R. Bjork
 * Revised 01/04/2000 by J. Senning
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define NUM_PHILOSOPHERS    5   /* Must be 5 */

/*
 * Macros to encapsulate the POSIX semaphore functions.
 */
#define semaphore_create(s,v)   sem_init( &s, 0, v )
#define semaphore_wait(s)   sem_wait( &s )
#define semaphore_signal(s) sem_post( &s )
#define semaphore_release(s)    sem_destroy( &s )
typedef sem_t semaphore;

/*
 * Each chopstick is represented by a semaphore.  We also need a semaphore
 * to control screen accesses so that only one thread at a time can write to
 * it, and another semaphore to control modifications of the shared variables.
 */
semaphore chopstick[NUM_PHILOSOPHERS];
semaphore screen;
semaphore mutex;
/*--------------------------------------------------------------------------*/

void obtain_chopsticks( int n )
/*
 * To obtain his chopsticks, a philosopher does a semaphore wait on each.
 * Alternating order prevents deadlock.
 */
{
    if (n % 2 == 0 ) {
    /* Even number: Left, then right */
    semaphore_wait( chopstick[(n+1) % NUM_PHILOSOPHERS] );
    semaphore_wait( chopstick[n] );
    } else {
    /* Odd number: Right, then left */
    semaphore_wait( chopstick[n] );
    semaphore_wait( chopstick[(n+1) % NUM_PHILOSOPHERS] );
    }
}

void release_chopsticks( int n )
/*
 * To release his chopsticks, a philosopher does a semaphore signal on each.
 * Order does not matter.
 */
{
    semaphore_signal( chopstick[n] );
    semaphore_signal( chopstick[(n+1) % NUM_PHILOSOPHERS] );
}

/*--------------------------------------------------------------------------*/

void philosopher( int *philosopher_data )
/*
 * Simulate a philosopher - endlessly cycling between eating and thinking
 * until his "life" is over.  Since this is called via pthread_create(), it
 * must accept a single argument which is a pointer to something.  In this
 * case the argument is a pointer to an array of two integers.  The first
 * is the philosopher number and the second is the duration (in seconds)
 * that the philosopher sits at the table.
 */
{
    int eat_count = 0;

    int n = philosopher_data[0];
    int duration = philosopher_data[1];

    while(eat_count < duration)
    {
    /* Hungry */
    obtain_chopsticks( n );
    /* Eating */
    eat_count++;
    sleep(rand() % 5 + 5);
    release_chopsticks(n);

    /* Think */
    sleep(rand() % 20 + 5);
    }

    /* Update the shared variable database */

    semaphore_wait(mutex);
    semaphore_signal( mutex );
    
    pthread_exit( NULL );
}

/*==========================================================================*/

int main( int argc, char *argv[] )
{
    pthread_t phil[NUM_PHILOSOPHERS];
    int philosopher_data[NUM_PHILOSOPHERS][2];
    int duration;
    int i;

    /*
     *  Set duration to 5
     */
    duration = 5;

    /*
     * Create semaphores to represent "one user at a time" forks, plus
     * one to protect the screen and one to protect shared variables.  
     */

    for ( i = 0; i < NUM_PHILOSOPHERS; i++ )
    {
    if ( semaphore_create( chopstick[i], 1 ) < 0 ) {
        fprintf( stderr, "cannot create chopstick semaphore\n" );
        exit( 1 );
    }
    }

    if ( semaphore_create( screen, 1 ) < 0 ) {
    fprintf( stderr, "cannot create screen semaphore\n" );
    exit( 1 );
    }

    if ( semaphore_create( mutex, 1 ) < 0 ) {
    fprintf( stderr, "cannot create mutex semaphore\n" );
    exit( 1 );
    }

    /*
     * Initialize the display and create a thread for each philosopher.
     */
    for ( i = 0; i < NUM_PHILOSOPHERS; i++ )
    {
    philosopher_data[i][0] = i;
    philosopher_data[i][1] = duration;
    if (pthread_create( &phil[i], NULL, (void *(*)(void *)) &philosopher,
                 &philosopher_data[i] ) != 0 ) {
        fprintf( stderr, "cannot create thread for philosopher %d\n", i );
        exit( 1 );
    }
    }

    /*
     * Wait for the philosophers to finish.
     */
    for ( i = 0; i < NUM_PHILOSOPHERS; i++ )
    {
    pthread_join( phil[i], NULL );
    }

    /*
     * Release semaphore resources.
     */
    for ( i = 0; i < NUM_PHILOSOPHERS; i++ )
    {
    semaphore_release( chopstick[i] );
    }
    semaphore_release( screen );
    semaphore_release( mutex );
    return 0;
}