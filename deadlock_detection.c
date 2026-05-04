#include <stdio.h>
#include <stdbool.h>

int main() {
    // Modify these variables to match your class examples
    int n = 5; // Number of processes (P0 to P4)
    int m = 3; // Number of resource types (e.g., A, B, C)

    // Current Allocation matrix
    int alloc[5][3] = { {0, 1, 0},   // P0
                        {2, 0, 0},   // P1
                        {3, 0, 3},   // P2
                        {2, 1, 1},   // P3
                        {0, 0, 2} }; // P4

    // Current Request matrix
    int request[5][3] = { {0, 0, 0},   // P0
                          {2, 0, 2},   // P1
                          {0, 0, 0},   // P2
                          {1, 0, 0},   // P3
                          {0, 0, 2} }; // P4

    // Available resources vector
    int avail[3] = {0, 0, 0}; 

    // Internal algorithm arrays
    int work[3];
    bool finish[5];
    int deadlocked_processes[5];
    int deadlock_count = 0;

    // Step 1: Initialize Work and Finish vectors
    for (int i = 0; i < m; i++) {
        work[i] = avail[i];
    }

    for (int i = 0; i < n; i++) {
        int is_alloc_zero = 1;
        for (int j = 0; j < m; j++) {
            if (alloc[i][j] != 0) {
                is_alloc_zero = 0;
                break;
            }
        }
        // If a process holds no resources, it cannot be part of a deadlock
        finish[i] = (is_alloc_zero == 1) ? true : false;
    }

    // Step 2: Detection Algorithm
    bool found;
    do {
        found = false;
        for (int i = 0; i < n; i++) {
            if (!finish[i]) {
                bool can_allocate = true;
                
                // Check if current request can be satisfied by available Work
                for (int j = 0; j < m; j++) {
                    if (request[i][j] > work[j]) {
                        can_allocate = false;
                        break;
                    }
                }

                // If request can be satisfied, process executes and releases resources
                if (can_allocate) {
                    for (int j = 0; j < m; j++) {
                        work[j] += alloc[i][j];
                    }
                    finish[i] = true;
                    found = true; // We made progress, so we loop again
                }
            }
        }
    } while (found);

    // Step 3: Check for Deadlock
    for (int i = 0; i < n; i++) {
        if (!finish[i]) {
            deadlocked_processes[deadlock_count++] = i;
        }
    }

    // Output Results
    if (deadlock_count > 0) {
        printf("System is in a DEADLOCK state.\n");
        printf("Deadlocked processes: ");
        for (int i = 0; i < deadlock_count; i++) {
            printf("P%d ", deadlocked_processes[i]);
        }
        printf("\n");
    } else {
        printf("System is NOT in a deadlock state. All processes can complete.\n");
    }

    return 0;
}