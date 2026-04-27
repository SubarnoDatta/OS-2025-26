#include <stdio.h>

struct Process {
    int id;
    int C;              // execution time per period
    int T;              // period
    int tokens;         // tokens held by this process
    int remaining;      // remaining exec time in current period
    int next_release;   // start of current period
    int deadline_misses;
};

struct GanttEntry {
    int pid;
    int start_time;
    int end_time;
};

int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }
int lcm(int a, int b) { return a / gcd(a, b) * b; }

void accept_processes(struct Process p[], int n, int total_tokens) {
    int assigned = 0;
    for (int i = 0; i < n; i++) {
        printf("Enter C (execution), T (period), Tokens for process %d: ", i + 1);
        scanf("%d %d %d", &p[i].C, &p[i].T, &p[i].tokens);
        p[i].id              = i + 1;
        p[i].remaining       = p[i].C;
        p[i].next_release    = 0;
        p[i].deadline_misses = 0;
        assigned            += p[i].tokens;
    }
    printf("\n  Tokens assigned to processes : %d\n", assigned);
    printf("  Unassigned (idle) tokens     : %d\n", total_tokens - assigned);
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
    printf("\nEnter total number of tokens: ");
    int total_tokens;
    scanf("%d", &total_tokens);

    printf("Enter the number of processes: ");
    int n;
    scanf("%d", &n);

    struct Process p[n];
    accept_processes(p, n, total_tokens);

    // Validate token assignment
    int assigned = 0;
    for (int i = 0; i < n; i++) assigned += p[i].tokens;
    if (assigned > total_tokens) {
        printf("\n  ERROR: Tokens assigned (%d) exceed total tokens (%d).\n",
               assigned, total_tokens);
        return 1;
    }

    // Hyperperiod = LCM of all periods
    int hyperperiod = p[0].T;
    for (int i = 1; i < n; i++)
        hyperperiod = lcm(hyperperiod, p[i].T);

    // Summary
    printf("\n--- Token Summary ---\n");
    printf("  Total tokens : %d\n", total_tokens);
    float U = 0;
    for (int i = 0; i < n; i++) {
        float share = (float)p[i].tokens / total_tokens * 100;
        float u     = (float)p[i].C / p[i].T;
        printf("  P%d: C=%d, T=%d, Tokens=%d/%d  =>  Share=%.1f%%,  C/T=%.3f\n",
               p[i].id, p[i].C, p[i].T, p[i].tokens, total_tokens, share, u);
        U += u;
    }
    float idle_share = (float)(total_tokens - assigned) / total_tokens * 100;
    if (total_tokens - assigned > 0)
        printf("  Idle tokens  : %d/%d  =>  Share=%.1f%%\n",
               total_tokens - assigned, total_tokens, idle_share);

    printf("\n  CPU Utilization : %.3f (%.1f%%)\n", U, U * 100);
    printf("  Hyperperiod     : %d ms\n", hyperperiod);
    if (U > 1.0)
        printf("  Result          : NOT FEASIBLE (U > 100%%) -- deadlines may be missed\n");
    else
        printf("  Result          : FEASIBLE\n");

    // Simulate tick by tick up to hyperperiod
    // Each tick: proportional share via deficit scheduling using token ratios
    // fair_share(i, t) = (tokens[i] / total_tokens) * t
    // Pick ready process with highest deficit = fair_share - cpu_received

    float cpu_received[n];
    for (int i = 0; i < n; i++) cpu_received[i] = 0;

    struct GanttEntry gantt[hyperperiod + 1];
    int g_count = 0;

    for (int t = 0; t < hyperperiod; t++) {

        // Period boundary: check deadline miss, re-release
        for (int i = 0; i < n; i++) {
            if (t > 0 && t == p[i].next_release + p[i].T) {
                if (p[i].remaining > 0)
                    p[i].deadline_misses++;
                p[i].remaining    = p[i].C;
                p[i].next_release = t;
            }
        }

        // Pick ready process with highest deficit from its token-based fair share
        int   chosen     = -1;
        float max_deficit = -99999;

        for (int i = 0; i < n; i++) {
            if (p[i].next_release <= t && p[i].remaining > 0) {
                // fair share based on token ratio out of total_tokens
                float fair  = ((float)p[i].tokens / total_tokens) * (t + 1);
                float deficit = fair - cpu_received[i];
                if (deficit > max_deficit) {
                    max_deficit = deficit;
                    chosen      = i;
                }
            }
        }

        // Record Gantt
        int cpid = (chosen == -1) ? 0 : p[chosen].id;
        if (g_count > 0 && gantt[g_count - 1].pid == cpid)
            gantt[g_count - 1].end_time++;
        else {
            gantt[g_count].pid        = cpid;
            gantt[g_count].start_time = t;
            gantt[g_count].end_time   = t + 1;
            g_count++;
        }

        if (chosen != -1) {
            cpu_received[chosen]++;
            p[chosen].remaining--;
        }
    }

    // Check misses at hyperperiod boundary
    for (int i = 0; i < n; i++)
        if (p[i].remaining > 0)
            p[i].deadline_misses++;

    // Deadline misses
    printf("\n--- Deadline Misses ---\n");
    int total_miss = 0;
    for (int i = 0; i < n; i++) {
        printf("  P%d (T=%2d, Tokens=%d): %d miss(es)\n",
               p[i].id, p[i].T, p[i].tokens, p[i].deadline_misses);
        total_miss += p[i].deadline_misses;
    }
    printf("  Total                : %d\n", total_miss);

    display_gantt_chart(gantt, g_count);

    return 0;
}