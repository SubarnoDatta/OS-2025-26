#include <stdio.h>
#include <stdbool.h>

int main() {
    // Number of processes and resources
    int n = 5; // P0, P1, P2, P3, P4
    int m = 3; // A, B, C

    // Allocation Matrix: Resources currently allocated to each process
    int alloc[5][3] = { {0, 1, 0},   // P0
                        {2, 0, 0},   // P1
                        {3, 0, 2},   // P2
                        {2, 1, 1},   // P3
                        {0, 0, 2} }; // P4

    // Max Matrix: Maximum demand of each process
    int max[5][3] = { {7, 5, 3},   // P0
                      {3, 2, 2},   // P1
                      {9, 0, 2},   // P2
                      {2, 2, 2},   // P3
                      {4, 3, 3} }; // P4

    // Available Resources Vector
    int avail[3] = {3, 3, 2}; 

    int need[5][3];
    
    // Step 1: Calculate the Need Matrix (Need = Max - Allocation)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            need[i][j] = max[i][j] - alloc[i][j];
        }
    }

    bool finish[5] = {0}; // Track which processes have finished
    int safeSeq[5];       // Store the safe sequence
    int work[3];          // Represents available resources during simulation
    
    // Initialize Work = Available
    for (int i = 0; i < m; i++) {
        work[i] = avail[i];
    }
    
    int count = 0; // Number of finished processes
    
    // Step 2: Safety Algorithm
    while (count < n) {
        bool found = false;
        
        for (int p = 0; p < n; p++) {
            // Find a process that is not finished and whose needs can be met
            if (!finish[p]) {
                int j;
                for (j = 0; j < m; j++) {
                    if (need[p][j] > work[j]) {
                        break; // Need is greater than available, move to next process
                    }
                }
                
                // If all needs of process p can be satisfied
                if (j == m) { 
                    // Process p executes, releasing its allocated resources
                    for (int k = 0; k < m; k++) {
                        work[k] += alloc[p][k];
                    }
                    
                    safeSeq[count++] = p;
                    finish[p] = true;
                    found = true;
                }
            }
        }
        
        // If no process was found in this pass, the system is not in a safe state
        if (!found) {
            printf("System is NOT in a safe state (Deadlock may occur).\n");
            return 0;
        }
    }
    
    // Step 3: Print the Safe Sequence
    printf("System is in a SAFE state.\nSafe Sequence: ");
    for (int i = 0; i < n; i++) {
        printf("P%d ", safeSeq[i]);
        if (i < n - 1) printf("-> ");
    }
    printf("\n");
    
    return 0;
}