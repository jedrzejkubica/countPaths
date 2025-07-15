#include <stdio.h>

#include "adjacency.h"
#include "compactAdjacency.h"
#include "pathCountsWithPredecessors.h"
#include "mem.h"


adjacencyMatrix *diamond4(void) {
    adjacencyMatrix *diamond4 = mallocOrDie(sizeof(adjacencyMatrix), "E: OOM for diamond4");
    diamond4->nbCols = 4;
    diamond4->weights = mallocOrDie(16 * sizeof(float), "E: OOM for diamond4 weights");
    for (size_t i = 0; i < 16; i++)
	diamond4->weights[i] = 0;
    diamond4->weights[1] = 1;
    diamond4->weights[2] = 1;
    diamond4->weights[4] = 1;
    diamond4->weights[7] = 1;
    diamond4->weights[8] = 1;
    diamond4->weights[11] = 1;
    diamond4->weights[13] = 1;
    diamond4->weights[14] = 1;
    // add a self-interaction 4->4 for testing
    diamond4->weights[15] = 1;
    return(diamond4);
}

int main(void) {
    adjacencyMatrix *diam4 = diamond4();
    printf("diam4 with its self-loop\n");
    printAdjacency(diam4);
    removeLoops(diam4);
    printf("diam4 without self-loops\n");
    printAdjacency(diam4);

    compactAdjacencyMatrix *diam4comp = adjacency2compact(diam4);

    pathCountsWithPredMatrix *diam4PathCountsD1 = buildFirstPathCounts(diam4comp);
    pathCountsWithPredMatrix *diam4PathCountsD2 = buildNextPathCounts(diam4PathCountsD1, diam4comp);
    freePathCountsWithPred(diam4PathCountsD1);
    freePathCountsWithPred(diam4PathCountsD2);

    freeCompactAdjacency(diam4comp);
    freeAdjacency(diam4);
    return(0);
}

