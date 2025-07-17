#include <stdio.h>

#include "pathCountsWithPredecessors.h"
#include "compactAdjacency.h"
#include "pathCounts.h"
#include "mem.h"


pathCountsMatrix *countPaths(pathCountsWithPredMatrix *pathCountsWithPred, compactAdjacencyMatrix *compact) {
    pathCountsMatrix *pathCounts = mallocOrDie(sizeof(pathCountsMatrix), "E: OOM for path counts matrix\n");
    
    unsigned int nbNodes = compact->nbNodes;
    unsigned int sumDegrees = compact->offsets[nbNodes];
    
    pathCounts->nbCols = compact->nbNodes;
    pathCounts->data = mallocOrDie(sizeof(float) * nbNodes * nbNodes, "E: OOM for path counts data\n");
    
    for (size_t i = 0; i < nbNodes; i++) {
        for (size_t j = 0; j < nbNodes; j++) {
            float sum = 0;
            for (size_t k = compact->offsets[j]; k < compact->offsets[j + 1]; k++)
                sum += pathCountsWithPred->data[i * sumDegrees + k];

            pathCounts->data[i * nbNodes + j] = sum;
        }
    }
    
    return pathCounts;
}

void printPathCounts(pathCountsMatrix *pathCounts) {
    for (size_t i = 0; i < pathCounts->nbCols; i++) {
        for (size_t j = 0; j < pathCounts->nbCols; j++)
            printf("%0.2f ", pathCounts->data[i * pathCounts->nbCols + j]);

        printf("\n");
    }
}

void freePathCounts(pathCountsMatrix *pathCounts) {
    free(pathCounts->data);
    free(pathCounts);
}