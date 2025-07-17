#include <stdio.h>

#include "adjacency.h"
#include "compactAdjacency.h"
#include "pathCountsWithPredecessors.h"
#include "pathCounts.h"
#include "mem.h"


adjacencyMatrix *diamond4(void) {
    adjacencyMatrix *diamond4 = mallocOrDie(sizeof(adjacencyMatrix), "E: OOM for diamond4");
    diamond4->nbCols = 4;
    diamond4->weights = mallocOrDie(16 * sizeof(float), "E: OOM for diamond4 weights");
    for (size_t i = 0; i < 16; i++)
	diamond4->weights[i] = 0;
    diamond4->weights[1] = 0.2;
    diamond4->weights[2] = 0.5;
    diamond4->weights[4] = 0.4;
    diamond4->weights[7] = 1;
    diamond4->weights[8] = 1;
    diamond4->weights[11] = 1;
    diamond4->weights[13] = 1;
    diamond4->weights[14] = 10;
    // add a self-interaction 4->4 for testing
    diamond4->weights[15] = 1;
    return(diamond4);
}

int main(void) {
    adjacencyMatrix *diam4 = diamond4();
    printf("diam4 with its self-loop\n");
    printAdjacency(diam4);

    compactAdjacencyMatrix *diam4comp = adjacency2compact(diam4);

    unsigned int maxDistance = 5;

    pathCountsWithPredMatrix *diam4PathCountsWithPred = buildFirstPathCounts(diam4comp);

    pathCountsWithPredMatrix *diam4Next = NULL;

    for (size_t i = 1; i < maxDistance; i++) {
        diam4Next = buildNextPathCounts(diam4PathCountsWithPred, diam4comp);
        
        pathCountsMatrix *diam4PathCounts = countPaths(diam4Next, diam4comp);
        printf("diam4 path counts D=%lu\n", i+1);
        printPathCounts(diam4PathCounts);
        freePathCounts(diam4PathCounts);

        freePathCountsWithPred(diam4PathCountsWithPred);
        diam4PathCountsWithPred = diam4Next;
    }

    freePathCountsWithPred(diam4Next);
    freeCompactAdjacency(diam4comp);
    freeAdjacency(diam4);

    return(0);
}

