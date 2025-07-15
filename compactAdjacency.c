#include "compactAdjacency.h"
#include "adjacency.h"
#include "mem.h"

compactAdjacencyMatrix *adjacency2compact(adjacencyMatrix *A) {
    compactAdjacencyMatrix *compact = mallocOrDie(sizeof(compactAdjacencyMatrix), "E: OOM for compact\n");

    compact->nbNodes = A->nbCols;
    compact->offsets = mallocOrDie(sizeof(size_t) * (A->nbCols + 1), "E: OOM for offsets\n");
    
    compact->offsets[0] = 0;
    size_t sumInDegrees = 0;
    for (unsigned int j = 0; j < A->nbCols; j++) {
        // in-degree of node j
        size_t inDegree = 0;
        for (unsigned int i = 0; i < A->nbCols; i++) {
            if (A->weights[i * A->nbCols + j] > 0) {
                inDegree++;
            }
        }
        sumInDegrees += inDegree;
        compact->offsets[j+1] = sumInDegrees;
    }

    compact->predecessors = mallocOrDie(sizeof(unsigned int) * sumInDegrees, "E: OOM for predecessors\n");
    compact->weights = mallocOrDie(sizeof(float) * sumInDegrees, "E: OOM for weights in compact\n");

    size_t idxPred = 0;
    for (unsigned int j = 0; j < A->nbCols; j++) {
        for (unsigned int i = 0; i < A->nbCols; i++) {
            if (A->weights[i * A->nbCols + j] > 0) {
                compact->predecessors[idxPred] = i;
                compact->weights[idxPred] = A->weights[i * A->nbCols + j];
                idxPred++;
            }
        }
    }

    return compact;
}

void freeCompactAdjacency(compactAdjacencyMatrix *compact) {
    free(compact->offsets);
    free(compact->predecessors);
    free(compact->weights);
    free(compact);
}
