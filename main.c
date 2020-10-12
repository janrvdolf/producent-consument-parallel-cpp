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
#include <string.h>

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

int is_end = 0;

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
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);

    /* Get the default attributes */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* init buffer */
    counter = 0;
}

/* Producer Thread */
void *producer(void *param) {
    long int thread_id = (long int) param;
    buffer_item item;

    printf("Inicializoval producer thread <%ld>\n", thread_id);

    while(!is_end) {
        /* sleep for a random period of time */
        int rNum = rand() / RAND_DIVISOR;
        //sleep(rNum);

        /* generate a random number */
        item = rand();

        // TODO
        // Is some empty space to fill?
        // If yes, continue, block otherwise.
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        int return_value = insert_item(item);
        printf("Producer <%ld> provedl insert(%d)\n", thread_id, item);
        pthread_mutex_unlock(&mutex);
        // Increases the value of the full semaphore by one.
        sem_post(&full);
    }

    printf("Producer <%ld> konci\n", thread_id);

    pthread_exit(NULL);
}

/* Consumer Thread */
void *consumer(void *param) {
    long int thread_id = (long int) param;
    buffer_item item;
    printf("Inicializoval consumer thread <%ld>\n", thread_id);

    while(!is_end) {
        /* sleep for a random period of time */
        int rNum = rand() / RAND_DIVISOR;
        //sleep(rNum);
        // TODO
        buffer_item  removed_item = 0;
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int return_value = remove_item(&removed_item);
        printf("Comsumer <%ld> provedl remove\n", thread_id);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }

    printf("Comsumer <%ld> konci\n", thread_id);

    pthread_exit(NULL);
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

    long int threads_cnt = 0; // `long int` proto, aby delka datoveho typu sedel s `void *`
    pthread_t * threads = (pthread_t *) malloc(sizeof(pthread_t) * (numCons + numProd));

    /* Initialize the app */
    initializeData();

    /* Create the producer threads */
    for(i = 0; i < numProd; i++) {
        /* Create the thread */
        int return_value = pthread_create(&threads[threads_cnt], &attr, (void * (*) (void *)) producer, (void *) threads_cnt);
        if (return_value == 0) {
            printf("Vlakno vytvoreno\n");
        } else {
            printf("pthread_create() fail %s", strerror(return_value));
        }

        threads_cnt++;
    }

    /* Create the consumer threads */
    for(i = 0; i < numCons; i++) {
        /* Create the thread */
        int return_value = pthread_create(&threads[threads_cnt], &attr, (void * (*) (void *)) consumer, (void *) threads_cnt);
        if (return_value == 0) {
            printf("Vlakno vytvoreno\n");
        } else {
            //printf("pthread_create() fail %s", strerror(return_value));
            printf("pthread_create() fail %s", strerror(return_value));
        }

        threads_cnt++;
    }

    /* Sleep for the specified amount of time in milliseconds */
    sleep(mainSleepTime);

    is_end = 1;

    threads_cnt = 0;
    for(i = 0; i < numProd; i++) {
        pthread_join(threads[threads_cnt], NULL);
        threads_cnt++;
    }
    for(i = 0; i < numCons; i++) {
        pthread_join(threads[threads_cnt], NULL);
        threads_cnt++;
    }

    sem_destroy(&full);
    sem_destroy(&empty);

    pthread_attr_destroy(&attr);

    free(threads);

    /* Exit the program */
    printf("Exit the program\n");
    exit(0);
}
