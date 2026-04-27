#include <stdio.h>

struct Process {
    int id, arrival_time, burst_time, remaining_time;
    int completion_time, waiting_time, turnaround_time, response_time;
    int started;
};

struct GanttEntry {
    int pid;        // process id (0 = idle)
    int start_time;
    int end_time;
};

void accept_processes(struct Process p[], int n) {
    for (int i = 0; i < n; i++) {
        printf("Enter arrival time and burst time for process %d: ", i + 1);
        scanf("%d %d", &p[i].arrival_time, &p[i].burst_time);
        p[i].id              = i + 1;
        p[i].remaining_time  = p[i].burst_time;
        p[i].completion_time = 0;
        p[i].waiting_time    = 0;
        p[i].turnaround_time = 0;
        p[i].response_time   = -1;
        p[i].started         = 0;
    }
}

void display_processes(struct Process p[], int n, int quantum) {
    printf("\nRound Robin Scheduling (Quantum = %d)\n", quantum);
    printf("| ID | AT | BT | CT | TAT | WT | RT |\n");
    for (int i = 0; i < n; i++) {
        printf("| %2d | %2d | %2d | %2d | %3d | %2d | %2d |\n",
               p[i].id, p[i].arrival_time, p[i].burst_time,
               p[i].completion_time, p[i].turnaround_time,
               p[i].waiting_time, p[i].response_time);
    }
}

void display_averages(struct Process p[], int n) {
    float tat = 0, wt = 0, rt = 0;
    for (int i = 0; i < n; i++) {
        tat += (float)p[i].turnaround_time / n;
        wt  += (float)p[i].waiting_time / n;
        rt  += (float)p[i].response_time / n;
    }
    printf("\nAverage Turnaround Time: %.3f ms", tat);
    printf("\nAverage Waiting Time   : %.3f ms", wt);
    printf("\nAverage Response Time  : %.3f ms\n", rt);
}

 void display_gantt_chart(struct GanttEntry gantt[], int g_count) {
    printf("\nGantt Chart:\n");

    // Top border
    for (int i = 0; i < g_count; i++) printf("+------");
    printf("+\n");

    // Labels
    for (int i = 0; i < g_count; i++) {
        if (gantt[i].pid == 0)
            printf("|  --  ");
        else
            printf("|  P%-2d ", gantt[i].pid);
    }
    printf("|\n");

    // Bottom border
    for (int i = 0; i < g_count; i++) printf("+------");
    printf("+\n");

    printf("%-7d", gantt[0].start_time);
    for (int i = 0; i < g_count; i++) {
        printf("%-7d", gantt[i].end_time);
    }
    printf("\n");
}

void round_robin(struct Process p[], int n, int quantum,
                 struct GanttEntry gantt[], int *g_count) {
    int queue[1000], front = 0, rear = 0;
    int in_queue[100] = {0};
    int current_time = 0, completed = 0;
    *g_count = 0;

    for (int i = 0; i < n; i++) {
        if (p[i].arrival_time == 0) {
            queue[rear++] = i;
            in_queue[i] = 1;
        }
    }

    while (completed != n) {
        if (front == rear) {
            int next_arrival = 99999;
            for (int i = 0; i < n; i++)
                if (p[i].remaining_time > 0 && p[i].arrival_time > current_time)
                    if (p[i].arrival_time < next_arrival)
                        next_arrival = p[i].arrival_time;

            gantt[*g_count].pid        = 0;
            gantt[*g_count].start_time = current_time;
            gantt[*g_count].end_time   = next_arrival;
            (*g_count)++;
            current_time = next_arrival;

            for (int i = 0; i < n; i++)
                if (p[i].arrival_time <= current_time && p[i].remaining_time > 0 && !in_queue[i]) {
                    queue[rear++] = i;
                    in_queue[i] = 1;
                }
            continue;
        }

        int idx = queue[front++];
        in_queue[idx] = 0;

        if (!p[idx].started) {
            p[idx].response_time = current_time - p[idx].arrival_time;
            p[idx].started = 1;
        }

        int exec_time = (p[idx].remaining_time < quantum) ? p[idx].remaining_time : quantum;

        // Every slice gets its own Gantt entry — no merging
        gantt[*g_count].pid        = p[idx].id;
        gantt[*g_count].start_time = current_time;
        gantt[*g_count].end_time   = current_time + exec_time;
        (*g_count)++;

        p[idx].remaining_time -= exec_time;
        current_time          += exec_time;

        for (int i = 0; i < n; i++)
            if (i != idx && p[i].arrival_time <= current_time &&
                p[i].remaining_time > 0 && !in_queue[i]) {
                queue[rear++] = i;
                in_queue[i] = 1;
            }

        if (p[idx].remaining_time == 0) {
            p[idx].completion_time = current_time;
            p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
            p[idx].waiting_time    = p[idx].turnaround_time - p[idx].burst_time;
            completed++;
        } else {
            queue[rear++] = idx;
            in_queue[idx] = 1;
        }
    }
}

int main() {
    printf("\nEnter the number of processes: ");
    int n;
    scanf("%d", &n);

    struct Process p[n];
    accept_processes(p, n);

    int quantum;
    printf("Enter the time quantum: ");
    scanf("%d", &quantum);

    int orig_burst[n];
    for (int i = 0; i < n; i++) orig_burst[i] = p[i].burst_time;

    struct GanttEntry gantt[1000];
    int g_count = 0;

    round_robin(p, n, quantum, gantt, &g_count);

    for (int i = 0; i < n; i++) p[i].burst_time = orig_burst[i];

    display_processes(p, n, quantum);
    display_averages(p, n);
    display_gantt_chart(gantt, g_count);

    return 0;
}
