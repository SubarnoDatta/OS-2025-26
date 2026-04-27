#include <stdio.h>

struct Process {
    int id, arrival_time, burst_time, priority;
    int completion_time, waiting_time, turnaround_time, response_time;
    int remaining_time;
    int is_completed;
};

void accept_processes(struct Process p[], int n) {
    for (int i = 0; i < n; i++) {
        printf("Enter arrival time, burst time, and priority for process %d: ", i+1);
        scanf("%d %d %d", &p[i].arrival_time, &p[i].burst_time, &p[i].priority);

        p[i].remaining_time = p[i].burst_time;

        p[i].id = i+1;

        p[i].is_completed = 0;
        p[i].completion_time = 0;
        p[i].waiting_time = 0;
        p[i].turnaround_time = 0;
        p[i].response_time = 0;
    }
}

void display_processes(struct Process p[], int n) {
    printf("\nPriority Non-Preemptive Scheduling\n");
    printf("| ID | AT | BT | Pri | CT | TAT | WT | RT |\n");
    for (int i = 0; i < n; i++) {
        printf("| %2d | %2d | %2d | %3d | %2d | %3d | %2d | %2d |\n",
               p[i].id,
               p[i].arrival_time,
               p[i].burst_time,
               p[i].priority,
               p[i].completion_time,
               p[i].turnaround_time,
               p[i].waiting_time,
               p[i].response_time);
    }
}

void display_averages(struct Process p[], int n) {
    float tat = 0, wt = 0, rt = 0;
    for (int i = 0; i < n; i++) {
        tat += (float)p[i].turnaround_time/n;
        wt += (float)p[i].waiting_time/n;
        rt += (float)p[i].response_time/n;
    }

    printf("\n\nAverage Turnaround Time: %.3f ms", tat);
    printf("\nAverage Waiting Time   : %.3f ms", wt);
    printf("\nAverage Response Time  : %.3f ms", rt);
}

int main() {
    printf("\nEnter the number of processes: ");
    int n;
    scanf("%d", &n);
    struct Process p[n];

    accept_processes(p, n);

    int current_time = 0, completed = 0;

    while (completed != n) {
        int highest_idx = -1;
        int highest_priority = 99999;

        for (int i = 0; i < n; i++) {
            if (p[i].arrival_time <= current_time && p[i].remaining_time > 0) {
                if (p[i].priority < highest_priority) {
                    highest_priority = p[i].priority;
                    highest_idx = i;
                }
                if (p[i].priority == highest_priority && highest_idx != -1 && p[i].arrival_time < p[highest_idx].arrival_time) {
                    highest_idx = i;
                }
            }
        }

        if (highest_idx != -1) {
            if (p[highest_idx].remaining_time == p[highest_idx].burst_time) {
                p[highest_idx].response_time = current_time - p[highest_idx].arrival_time;
            }

            p[highest_idx].remaining_time--;
            current_time++;

            if (p[highest_idx].remaining_time == 0) {
                p[highest_idx].completion_time = current_time;
                p[highest_idx].turnaround_time = p[highest_idx].completion_time - p[highest_idx].arrival_time;
                p[highest_idx].waiting_time = p[highest_idx].turnaround_time - p[highest_idx].burst_time;
                completed++;
            }
        } else {
            current_time++;
        }
    }

    display_processes(p, n);
    display_averages(p, n);

    return 0;
}
