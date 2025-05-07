#include "kernighan_lin.h"
#include <stdlib.h>
#include <stdbool.h>

void kernighanLin(Graph *graph, int *partition) {
    int n = graph->numVert;

    // Initialization: First half goes to group 0, second goes to group 1
    for (int i = 0; i < n; i++) {
        partition[i] = (i < n / 2) ? 0 : 1;
    }

    bool changed = true;

    while (changed) {
        changed = false;
        int bestGain = -1;
        int bestA = -1, bestB = -1;

        for (int a = 0; a < n; a++) {
            if (partition[a] != 0) continue;

            for (int b = 0; b < n; b++) {
                if (partition[b] != 1) continue;

                int gain = 0;

                Node *na = graph->list[a];
                while (na) {
                    if (na->vertex == b) gain += 1;
                    else if (partition[na->vertex] == 0) gain -= 1;
                    na = na->next;
                }

                Node *nb = graph->list[b];
                while (nb) {
                    if (nb->vertex == a) gain += 1;
                    else if (partition[nb->vertex] == 1) gain -= 1;
                    nb = nb->next;
                }

                if (gain > bestGain) {
                    bestGain = gain;
                    bestA = a;
                    bestB = b;
                }
            }
        }

        if (bestGain > 0 && bestA != -1 && bestB != -1) {
            partition[bestA] = 1;
            partition[bestB] = 0;
            changed = true;
        }
    }
}
