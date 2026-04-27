#include <stdio.h>

struct Process {
    int id;
    int C;              // execution time
    int T;              // period
    int D;              // relative deadline (D <= T)
    int remaining;      // remaining in current period
    int next_release;   // next release time
    int next_deadline;  // absolute deadline of current instance
    int deadline_misses;
};

struct GanttEntry {
    int pid;
    int start_time;
    int end_time;
};

int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }
int lcm(int a, int b) { return a / gcd(a, b) * b; }

void accept_processes(struct Process p[], int n) {
    for (int i = 0; i < n; i++) {
        printf("Enter C, T (period), D (relative deadline) for process %d: ", i + 1);
        scanf("%d %d %d", &p[i].C, &p[i].T, &p[i].D);
        p[i].id              = i + 1;
        p[i].remaining       = p[i].C;
        p[i].next_release    = 0;
        p[i].next_deadline   = p[i].D;  // first absolute deadline = D
        p[i].deadline_misses = 0;
    }
}

void display_gantt_chart(struct GanttEntry gantt[], int g_count) {
    printf("\nGantt Chart:\n");
    for (int i = 0; i < g_count; i++) printf("+------");
    printf("+\n");
    for (int i = 0; i < g_count; i++) {
        if (gantt[i].pid == 0) printf("|  --  ");
        else                   printf("|  P%-2d ", gantt[i].pid);
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

    // Hyperperiod = LCM of all periods T
    int hyperperiod = p[0].T;
    for (int i = 1; i < n; i++)
        hyperperiod = lcm(hyperperiod, p[i].T);

    // Feasibility check: U = sigma(Ci / Di)
    printf("\n--- Feasibility Check ---\n");
    float U = 0;
    for (int i = 0; i < n; i++) {
        float u = (float)p[i].C / p[i].D;
        printf("  P%d: C=%d, T=%d, D=%d  =>  C/D = %.3f\n",
               p[i].id, p[i].C, p[i].T, p[i].D, u);
        U += u;
    }
    printf("\n  CPU Utilization : %.3f (%.1f%%)\n", U, U * 100);
    printf("  Hyperperiod     : %d ms\n", hyperperiod);
    if (U > 1.0)
        printf("  Result          : NOT FEASIBLE (U > 100%%) -- deadlines may be missed\n");
    else
        printf("  Result          : FEASIBLE\n");

    // Simulate tick by tick up to hyperperiod
    struct GanttEntry gantt[hyperperiod + 1];
    int g_count = 0;

    for (int t = 0; t < hyperperiod; t++) {

        // At each period boundary: check deadline miss, re-release
        for (int i = 0; i < n; i++) {
            if (t == p[i].next_release + p[i].T && t > 0) {
                if (p[i].remaining > 0)
                    p[i].deadline_misses++;
                p[i].remaining     = p[i].C;
                p[i].next_release  = t;
                p[i].next_deadline = t + p[i].D;  // new abs deadline = release + D
            }
        }

        // EDF: pick ready process with earliest absolute deadline
        int chosen  = -1;
        int earliest = 99999;
        for (int i = 0; i < n; i++) {
            if (p[i].next_release <= t && p[i].remaining > 0) {
                if (p[i].next_deadline < earliest) {
                    earliest = p[i].next_deadline;
                    chosen   = i;
                }
            }
        }

        // Record Gantt (extend if same process)
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

    // Deadline misses summary
    printf("\n--- Deadline Misses ---\n");
    int total = 0;
    for (int i = 0; i < n; i++) {
        printf("  P%d (T=%2d, D=%2d): %d miss(es)\n",
               p[i].id, p[i].T, p[i].D, p[i].deadline_misses);
        total += p[i].deadline_misses;
    }
    printf("  Total           : %d\n", total);

    display_gantt_chart(gantt, g_count);

    return 0;
}