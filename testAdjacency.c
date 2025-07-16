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

    pathCountsWithPredMatrix *diam4PathCountsWithPredD1 = buildFirstPathCounts(diam4comp);
    pathCountsWithPredMatrix *diam4PathCountsWithPredD2 = buildNextPathCounts(diam4PathCountsWithPredD1, diam4comp);
    pathCountsWithPredMatrix *diam4PathCountsWithPredD3 = buildNextPathCounts(diam4PathCountsWithPredD2, diam4comp);
    pathCountsWithPredMatrix *diam4PathCountsWithPredD4 = buildNextPathCounts(diam4PathCountsWithPredD3, diam4comp);
    pathCountsWithPredMatrix *diam4PathCountsWithPredD5 = buildNextPathCounts(diam4PathCountsWithPredD4, diam4comp);

    pathCountsMatrix *diam4PathCounts = countPaths(diam4PathCountsWithPredD5, diam4comp);
    printf("diam4 path counts d=5\n");
    printPathCounts(diam4PathCounts);

    freePathCounts(diam4PathCounts);
    freePathCountsWithPred(diam4PathCountsWithPredD5);
    freePathCountsWithPred(diam4PathCountsWithPredD4);
    freePathCountsWithPred(diam4PathCountsWithPredD3);
    freePathCountsWithPred(diam4PathCountsWithPredD2);
    freePathCountsWithPred(diam4PathCountsWithPredD1);
    freeCompactAdjacency(diam4comp);
    freeAdjacency(diam4);

    return(0);

    // unsigned int maxDistance = 4;

    // for (size_t i = 0; i < maxDistance; i++) {
    //     pathCountsWithPredMatrix *diam4Next = buildNextPathCounts(diam4PathCountsWithPred, diam4comp);
    //     freePathCountsWithPred(diam4PathCountsWithPred);
    //     pathCountsWithPredMatrix *diam4PathCountsWithPred = diam4Next;
    //     freePathCountsWithPred(diam4Next);
    // }
    
    // pathCountsMatrix *diam4PathCounts = countPaths(diam4PathCountsWithPred, diam4comp);
    // printf("diam4 path counts\n");
    // printPathCounts(diam4PathCounts);
}

