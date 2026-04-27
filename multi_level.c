#include <stdio.h>

struct Process {
    int id, arrival_time, burst_time;
    int completion_time, waiting_time, turnaround_time, response_time;
    int type; // 0 = System, 1 = User
};

struct GanttEntry {
    int pid;
    int queue;      // 1 = system, 2 = user, 0 = idle
    int start_time;
    int end_time;
};

void accept_processes(struct Process p[], int n) {
    for (int i = 0; i < n; i++) {
        printf("\nProcess %d\n", i + 1);
        printf("  Type (0 = System, 1 = User): ");
        scanf("%d", &p[i].type);
        printf("  Arrival time : ");
        scanf("%d", &p[i].arrival_time);
        printf("  Burst time   : ");
        scanf("%d", &p[i].burst_time);
        p[i].id              = i + 1;
        p[i].completion_time = 0;
        p[i].waiting_time    = 0;
        p[i].turnaround_time = 0;
        p[i].response_time   = 0;
    }
}

void display_queue(struct Process p[], int n, const char *label) {
    printf("\n%s\n", label);
    printf("| ID | Type   | AT | BT | CT | TAT | WT | RT |\n");
    for (int i = 0; i < n; i++) {
        printf("| %2d | %-6s | %2d | %2d | %2d | %3d | %2d | %2d |\n",
               p[i].id,
               p[i].type == 0 ? "System" : "User",
               p[i].arrival_time, p[i].burst_time,
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
    printf("  Average Turnaround Time: %.3f ms\n", tat);
    printf("  Average Waiting Time   : %.3f ms\n", wt);
    printf("  Average Response Time  : %.3f ms\n", rt);
}

 void display_gantt_chart(struct GanttEntry gantt[], int g_count) {
    printf("\nGantt Chart:\n");

    for (int i = 0; i < g_count; i++) printf("+--------");
    printf("+\n");

    for (int i = 0; i < g_count; i++) {
        if (gantt[i].pid == 0)
            printf("|  ----  ");
        else
            printf("| [%c] P%d ", gantt[i].queue == 1 ? 'S' : 'U', gantt[i].pid);
    }
    printf("|\n");

    for (int i = 0; i < g_count; i++) printf("+--------");
    printf("+\n");

    printf("%-9d", gantt[0].start_time);
    for (int i = 0; i < g_count; i++)
        printf("%-9d", gantt[i].end_time);
    printf("\n");
}

void fcfs(struct Process p[], int indices[], int count, int *current_time,
          struct GanttEntry gantt[], int *g_count, int queue_level) {

    for (int i = 1; i < count; i++) {
        int key = indices[i], j = i - 1;
        while (j >= 0 && p[indices[j]].arrival_time > p[key].arrival_time) {
            indices[j + 1] = indices[j];
            j--;
        }
        indices[j + 1] = key;
    }

    for (int i = 0; i < count; i++) {
        int idx = indices[i];

        if (*current_time < p[idx].arrival_time) {
            gantt[*g_count].pid        = 0;
            gantt[*g_count].queue      = 0;
            gantt[*g_count].start_time = *current_time;
            gantt[*g_count].end_time   = p[idx].arrival_time;
            (*g_count)++;
            *current_time = p[idx].arrival_time;
        }

        gantt[*g_count].pid        = p[idx].id;
        gantt[*g_count].queue      = queue_level;
        gantt[*g_count].start_time = *current_time;
        gantt[*g_count].end_time   = *current_time + p[idx].burst_time;
        (*g_count)++;

        p[idx].response_time   = *current_time - p[idx].arrival_time;
        *current_time         += p[idx].burst_time;
        p[idx].completion_time = *current_time;
        p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
        p[idx].waiting_time    = p[idx].turnaround_time - p[idx].burst_time;
    }
}

int main() {
    printf("\nEnter the number of processes: ");
    int n;
    scanf("%d", &n);

    struct Process p[n];
    accept_processes(p, n);

    int sys_idx[n], user_idx[n];
    int sys_count = 0, user_count = 0;

    for (int i = 0; i < n; i++) {
        if (p[i].type == 0) sys_idx[sys_count++]   = i;
        else                 user_idx[user_count++] = i;
    }

    int current_time = 0;
    struct GanttEntry gantt[500];
    int g_count = 0;

    if (sys_count > 0)
        fcfs(p, sys_idx, sys_count, &current_time, gantt, &g_count, 1);
    if (user_count > 0)
        fcfs(p, user_idx, user_count, &current_time, gantt, &g_count, 2);

    printf("\n=== Multi-Level Queue Scheduling (System > User, FCFS each) ===");

    struct Process sys_p[sys_count > 0 ? sys_count : 1];
    struct Process user_p[user_count > 0 ? user_count : 1];
    for (int i = 0; i < sys_count; i++)  sys_p[i]  = p[sys_idx[i]];
    for (int i = 0; i < user_count; i++) user_p[i] = p[user_idx[i]];

    if (sys_count > 0) {
        display_queue(sys_p, sys_count, "Queue 1 - System Processes (Higher Priority)");
        printf("\nAverages for System Processes:\n");
        display_averages(sys_p, sys_count);
    }
    if (user_count > 0) {
        display_queue(user_p, user_count, "Queue 2 - User Processes (Lower Priority)");
        printf("\nAverages for User Processes:\n");
        display_averages(user_p, user_count);
    }

    printf("\nOverall Averages (All %d Processes):\n", n);
    display_averages(p, n);

    display_gantt_chart(gantt, g_count);

    return 0;
}
