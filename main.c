// Basically, we are going to have a program that creates an N number of producer and consumer threads.
// The job of the producer will be to generate a random number and place it in a bound-buffer. The role
// of the consumer will be to remove items from the bound-buffer and print them to the screen. Sounds
// simple right? Well, yes and no. Remember the big issue here is concurrency so we will be using
// semaphores to help prevent any issues that might occur. To double our efforts we will also be using
// a pthread mutex lock to further guarantee synchronization.
// The user will pass in three arguments to start to application:
// <INT, time for the main method to sleep before termination> <INT, Number of producer threads> <INT, number of consumer threads>
// http://macboypro.com/blog/2009/06/29/producer-consumer-problem-in-c-using-pthreadsbounded-buffersemaphores/

#include <stdlib.h>
#include <stdio.h>

#include <pthread.h>
#include <semaphore.h>

// on windows only
//#include <windows.h>
#include <unistd.h>

#define RAND_DIVISOR 100000000
#define TRUE 1

typedef int buffer_item;
#define BUFFER_SIZE 5

/* The mutex lock */
pthread_mutex_t mutex;

/* the semaphores */
sem_t full, empty;

/* the buffer */
buffer_item buffer[BUFFER_SIZE];

/* buffer counter */
int counter;

pthread_t tid;       //Thread ID
pthread_attr_t attr; //Set of thread attributes

void *producer(void *param); /* the producer thread */
void *consumer(void *param); /* the consumer thread */

/* Add an item to the buffer */
int insert_item(buffer_item item) {
    /* When the buffer is not full add the item
       and increment the counter*/
    if(counter < BUFFER_SIZE) {
        buffer[counter] = item;
        counter++;
        return 0;
    }
    else { /* Error the buffer is full */
        return -1;
    }
}

/* Remove an item from the buffer */
int remove_item(buffer_item *item) {
    /* When the buffer is not empty remove the item
       and decrement the counter */
    if(counter > 0) {
        *item = buffer[(counter-1)];
        counter--;
        return 0;
    }
    else { /* Error buffer empty */
        return -1;
    }
}


void initializeData() {

    /* Create the mutex lock */
    pthread_mutex_init(&mutex, NULL);

    /* Create the full semaphore and initialize to 0 */
    /* Create the empty semaphore and initialize to BUFFER_SIZE */

    // TODO

    /* Get the default attributes */
    pthread_attr_init(&attr);

    /* init buffer */
    counter = 0;
}

/* Producer Thread */
void *producer(void *param) {
    buffer_item item;

    while(TRUE) {
        /* sleep for a random period of time */
        int rNum = rand() / RAND_DIVISOR;
        sleep(rNum);

        /* generate a random number */
        item = rand();

        // TODO
    }
}

/* Consumer Thread */
void *consumer(void *param) {
    buffer_item item;

    while(TRUE) {
        /* sleep for a random period of time */
        int rNum = rand() / RAND_DIVISOR;
        sleep(rNum);

        // TODO
    }
}


int main(int argc, char *argv[]) {
    /* Loop counter */
    int i;
    int mainSleepTime; /* Time in seconds for main to sleep */
    int numProd; /* Number of producer threads */
    int numCons; /* Number of consumer threads */

    /* Verify the correct number of arguments were passed in */
    if(argc != 4) {
        fprintf(stderr, "USAGE:./main.out <INT> <INT> <INT>\n");
        //exit(1);
        mainSleepTime = 1000; /* Time in seconds for main to sleep */
        numProd = 2; /* Number of producer threads */
        numCons = 2; /* Number of consumer threads */
    }
    else {
        mainSleepTime = atoi(argv[1]); /* Time in seconds for main to sleep */
        numProd = atoi(argv[2]); /* Number of producer threads */
        numCons = atoi(argv[3]); /* Number of consumer threads */
    }

    /* Initialize the app */
    initializeData();

    /* Create the producer threads */
    for(i = 0; i < numProd; i++) {
        /* Create the thread */
        pthread_create(&tid, &attr, producer,NULL);
    }

    /* Create the consumer threads */
    for(i = 0; i < numCons; i++) {
        /* Create the thread */
        pthread_create(&tid, &attr, consumer,NULL);
    }

    /* Sleep for the specified amount of time in milliseconds */
    sleep(mainSleepTime);

    /* Exit the program */
    printf("Exit the program\n");
    exit(0);
}
