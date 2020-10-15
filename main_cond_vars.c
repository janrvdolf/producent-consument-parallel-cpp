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

/* the conditional vars */
//sem_t full, empty;
pthread_cond_t full, empty;

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
    if(counter < (BUFFER_SIZE - 1)) {
        counter++;
        buffer[counter] = item;
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
    if(counter >= 0) {
        *item = buffer[counter];
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
//    sem_init(&full, 0, 0);
//    sem_init(&empty, 0, BUFFER_SIZE);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);

    /* Get the default attributes */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* init buffer */
    counter = -1;
}

/* Producer Thread */
void *producer(void *param) {
    long int thread_id = (long int) param;
    buffer_item item;

    printf("Inicializoval producer thread <%ld>\n", thread_id);
    while(!is_end) {
        /* sleep for a random period of time */
        int rNum = rand() / RAND_DIVISOR;
        sleep(rNum);

        /* generate a random number */
        item = rand();

        // Is some empty space to fill?
        // If yes, continue, block otherwise.
        pthread_mutex_lock(&mutex);
        // TODO producer is not going to produce if the buffer is full
        while (counter == (BUFFER_SIZE - 1)) {
            pthread_cond_wait(&full, &mutex);
        }

        int return_value = insert_item(item);
        if (return_value != 0) {
            printf("Producer <%ld> neprovedl insert\n", thread_id);
        } else {
            printf("Producer <%ld> provedl insert hodnoty %d na pozici %d\n", thread_id, item, counter);
        }

        pthread_cond_signal(&empty);

        pthread_mutex_unlock(&mutex);

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
        sleep(rNum);

        // TODO do not consume if the buffer is empty

        buffer_item  removed_item = 0;

        pthread_mutex_lock(&mutex);

        while (counter < 0) {
            pthread_cond_wait(&empty, &mutex);
        }

        int return_value = remove_item(&removed_item);
        if (return_value != 0) {
            printf("Comsumer <%ld> neprovedl remove\n", thread_id);
        } else {
            printf("Comsumer <%ld> provedl remove hodnoty %d z pozice %d\n", thread_id, removed_item, counter + 1);
        }

        pthread_cond_signal(&full);

        pthread_mutex_unlock(&mutex);

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

    pthread_attr_destroy(&attr);

    free(threads);

    /* Exit the program */
    printf("Exit the program\n");
    exit(0);
}
