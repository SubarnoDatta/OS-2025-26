#include <stdio.h>

struct Process {
    int id;
    int C;              // execution time
    int T;              // period
    int remaining;      // remaining exec time in current period
    int next_deadline;  // next deadline
    int deadline_misses;
};

struct GanttEntry {
    int pid;            // 0 = idle
    int start_time;
    int end_time;
};

int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }
int lcm(int a, int b) { return a / gcd(a, b) * b; }

void accept_processes(struct Process p[], int n) {
    for (int i = 0; i < n; i++) {
        printf("Enter C (execution time) and T (period) for process %d: ", i + 1);
        scanf("%d %d", &p[i].C, &p[i].T);
        p[i].id              = i + 1;
        p[i].remaining       = p[i].C;
        p[i].next_deadline   = p[i].T;
        p[i].deadline_misses = 0;
    }
}

void display_gantt_chart(struct GanttEntry gantt[], int g_count) {
    printf("\nGantt Chart:\n");

    for (int i = 0; i < g_count; i++) printf("+------");
    printf("+\n");

    for (int i = 0; i < g_count; i++) {
        if (gantt[i].pid == 0)
            printf("|  --  ");
        else
            printf("|  P%-2d ", gantt[i].pid);
    }
    printf("|\n");

    for (int i = 0; i < g_count; i++) printf("+------");
    printf("+\n");

    printf("%-7d", gantt[0].start_time);
    for (int i = 0; i < g_count; i++)
        printf("%-7d", gantt[i].end_time);
    printf("\n");
}

int main() {
    printf("\nEnter the number of processes: ");
    int n;
    scanf("%d", &n);

    struct Process p[n];
    accept_processes(p, n);

    // Priority order: shorter T = higher priority
    int prio[n];
    for (int i = 0; i < n; i++) prio[i] = i;
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (p[prio[i]].T > p[prio[j]].T) {
                int tmp = prio[i]; prio[i] = prio[j]; prio[j] = tmp;
            }

    // Hyperperiod = LCM of all periods
    int hyperperiod = p[0].T;
    for (int i = 1; i < n; i++)
        hyperperiod = lcm(hyperperiod, p[i].T);

    printf("\nHyperperiod (LCM): %d ms\n", hyperperiod);
    printf("\nPriority assignment (1 = highest):\n");
    for (int k = 0; k < n; k++)
        printf("  Priority %d -> P%d (T=%d)\n", k + 1, p[prio[k]].id, p[prio[k]].T);

    // Simulate tick by tick up to hyperperiod
    struct GanttEntry gantt[hyperperiod + 1];
    int g_count = 0;

    for (int t = 0; t < hyperperiod; t++) {

        // At each period boundary: check deadline miss, re-release
        for (int i = 0; i < n; i++) {
            if (t == p[i].next_deadline) {
                if (p[i].remaining > 0)
                    p[i].deadline_misses++;
                p[i].remaining     = p[i].C;
                p[i].next_deadline = t + p[i].T;
            }
        }

        // Pick highest priority ready process
        int chosen = -1;
        for (int k = 0; k < n; k++) {
            int i = prio[k];
            if (p[i].remaining > 0) { chosen = i; break; }
        }

        // Record Gantt (extend last entry if same pid)
        int cpid = (chosen == -1) ? 0 : p[chosen].id;
        if (g_count > 0 && gantt[g_count - 1].pid == cpid)
            gantt[g_count - 1].end_time++;
        else {
            gantt[g_count].pid        = cpid;
            gantt[g_count].start_time = t;
            gantt[g_count].end_time   = t + 1;
            g_count++;
        }

        if (chosen != -1)
            p[chosen].remaining--;
    }

    // Check misses at hyperperiod boundary
    for (int i = 0; i < n; i++)
        if (p[i].remaining > 0)
            p[i].deadline_misses++;

    // Schedulability check
    printf("\n--- Schedulability Check ---\n");
    float utilization = 0;
    for (int i = 0; i < n; i++) {
        float u = (float)p[i].C / p[i].T;
        printf("  P%d: C=%d, T=%d  =>  C/T = %.3f\n", p[i].id, p[i].C, p[i].T, u);
        utilization += u;
    }

    float bounds[] = {0, 1.000, 0.828, 0.780, 0.757, 0.743,
                         0.735, 0.729, 0.724, 0.720, 0.718};
    float ll_bound = (n <= 10) ? bounds[n] : 0.693;

    printf("\n  CPU Utilization     : %.3f (%.1f%%)\n", utilization, utilization * 100);
    printf("  Liu & Layland Bound : %.3f (%.1f%%)\n", ll_bound, ll_bound * 100);

    if (utilization > 1.0)
        printf("  Result              : NOT SCHEDULABLE (U > 100%%)\n");
    else if (utilization <= ll_bound)
        printf("  Result              : SCHEDULABLE\n");
    else
        printf("  Result              : MAY BE SCHEDULABLE (U between bound and 100%%)\n");

    // Deadline misses
    printf("\n--- Deadline Misses ---\n");
    int total = 0;
    for (int k = 0; k < n; k++) {
        int i = prio[k];
        printf("  P%d (T=%2d): %d miss(es)\n", p[i].id, p[i].T, p[i].deadline_misses);
        total += p[i].deadline_misses;
    }
    printf("  Total     : %d\n", total);

    display_gantt_chart(gantt, g_count);

    return 0;
}