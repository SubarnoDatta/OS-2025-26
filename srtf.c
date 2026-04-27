#include <stdio.h>

struct Process {
    int id, arrival_time, burst_time, remaining_time;
    int completion_time, waiting_time, turnaround_time, response_time;
};

void accept_processes(struct Process p[], int n) {
    for (int i = 0; i < n; i++) {
        printf("Enter arrival time and burst time for process %d: ", i+1);
        scanf("%d %d", &p[i].arrival_time, &p[i].burst_time);

        p[i].id = i+1;

        p[i].remaining_time = p[i].burst_time;

        p[i].completion_time = 0;
        p[i].waiting_time = 0;
        p[i].turnaround_time = 0;
        p[i].response_time = 0;
    }
}

void display_processes(struct Process p[], int n) {
    printf("\nSJF Preemptive (SRTF)\n");
    printf("| ID | AT | BT | CT | TAT | WT | RT |\n");
    for (int i = 0; i < n; i++) {
        printf("| %2d | %2d | %2d | %2d | %3d | %2d | %2d |\n",
               p[i].id,
               p[i].arrival_time,
               p[i].burst_time,
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
        int min_idx = -1;
        int min_remaining = 99999;

        for (int i = 0; i < n; i++) {
            if (p[i].arrival_time <= current_time && p[i].remaining_time > 0) {
                if (p[i].remaining_time < min_remaining) {
                    min_remaining = p[i].remaining_time;
                    min_idx = i;
                }

                if (p[i].remaining_time == min_remaining && min_idx != -1 && p[i].arrival_time < p[min_idx].arrival_time) {
                    min_idx = i;
                }
            }
        }

        if (min_idx != -1) {
            if (p[min_idx].remaining_time == p[min_idx].burst_time) {
                p[min_idx].response_time = current_time - p[min_idx].arrival_time;
            }

            p[min_idx].remaining_time--;
            current_time++;

            if (p[min_idx].remaining_time == 0) {
                p[min_idx].completion_time = current_time;
                p[min_idx].turnaround_time = p[min_idx].completion_time - p[min_idx].arrival_time;
                p[min_idx].waiting_time = p[min_idx].turnaround_time - p[min_idx].burst_time;
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
