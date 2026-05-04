#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define N 5 // Number of philosophers

sem_t chopstick[N];

void* philosopher(void* num) {
    int id = *(int*)num;

    // Define left and right chopsticks
    int left = id;
    int right = (id + 1) % N;

    printf("Philosopher %d is thinking.\n", id);
    sleep(1); // Simulate thinking time

    printf("Philosopher %d is hungry.\n", id);

    // Asymmetric picking to prevent deadlock:
    // Even philosophers pick up Right then Left.
    // Odd philosophers pick up Left then Right.
    if (id % 2 == 0) {
        sem_wait(&chopstick[right]);
        sem_wait(&chopstick[left]);
    } else {
        sem_wait(&chopstick[left]);
        sem_wait(&chopstick[right]);
    }

    printf("Philosopher %d is eating.\n", id);
    sleep(2); // Simulate eating time

    // Put down chopsticks
    sem_post(&chopstick[left]);
    sem_post(&chopstick[right]);

    printf("Philosopher %d finished eating and put down chopsticks.\n", id);

    return NULL;
}

int main() {
    pthread_t tid[N];
    int phil_id[N];

    // Initialize semaphores (chopsticks) to 1 (available)
    for (int i = 0; i < N; i++) {
        sem_init(&chopstick[i], 0, 1);
        phil_id[i] = i; // Assign IDs 0 to 4
    }

    // Create philosopher threads
    for (int i = 0; i < N; i++) {
        pthread_create(&tid[i], NULL, philosopher, &phil_id[i]);
    }

    // Wait for threads to complete (they finish after one meal in this simulation)
    for (int i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }

    // Destroy semaphores
    for (int i = 0; i < N; i++) {
        sem_destroy(&chopstick[i]);
    }
    
    printf("Simulation complete.\n");
    return 0;
}