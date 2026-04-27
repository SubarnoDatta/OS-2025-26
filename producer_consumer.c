#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t mutex;
sem_t empty;
sem_t full;

void* producer(void* arg) {
    int producer_id = *((int*)arg);
    
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = rand() % 100; // Generate a random item
        
        // wait(empty) - Wait if the buffer is full
        sem_wait(&empty);
        
        // wait(mutex) - Enter critical section
        sem_wait(&mutex);
        
        // Add item to buffer
        buffer[in] = item;
        printf("Producer %d produced item %d at index %d\n", producer_id, item, in);
        in = (in + 1) % BUFFER_SIZE;
        
        // signal(mutex) - Leave critical section
        sem_post(&mutex);
        
        // signal(full) - Increment the count of full slots
        sem_post(&full);
        
        sleep(1); // Simulate time taken to produce
    }
    return NULL;
}

// Consumer thread function
void* consumer(void* arg) {
    int consumer_id = *((int*)arg);
    
    for (int i = 0; i < NUM_ITEMS; i++) {
        // wait(full) - Wait if the buffer is empty
        sem_wait(&full);
        
        // wait(mutex) - Enter critical section
        sem_wait(&mutex);
        
        // Remove item from buffer
        int item = buffer[out];
        printf("Consumer %d consumed item %d from index %d\n", consumer_id, item, out);
        out = (out + 1) % BUFFER_SIZE;
        
        // signal(mutex) - Leave critical section
        sem_post(&mutex);
        
        // signal(empty) - Increment the count of empty slots
        sem_post(&empty);
        
        sleep(2); // Simulate time taken to consume
    }
    return NULL;
}

int main() {
    pthread_t prod_thread, cons_thread;
    int prod_id = 1, cons_id = 1;

    sem_init(&mutex, 0, 1);           // mutex initialized to 1
    sem_init(&empty, 0, BUFFER_SIZE); // empty initialized to buffer size
    sem_init(&full, 0, 0);            // full initialized to 0

    // Create threads
    pthread_create(&prod_thread, NULL, producer, &prod_id);
    pthread_create(&cons_thread, NULL, consumer, &cons_id);

    // Wait for threads to finish
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    printf("Simulation complete.\n");
    return 0;
}