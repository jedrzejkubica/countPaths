#include "compactAdjacency.h"
#include "adjacency.h"
#include "mem.h"

compactAdjacencyMatrix *adjacency2compact(adjacencyMatrix *A) {
    compactAdjacencyMatrix *compact = mallocOrDie(sizeof(compactAdjacencyMatrix), "E: OOM for cost\n");

    compact->nbNodes = A->nbCols;
    compact->offsets = mallocOrDie(sizeof(unsigned int) * (A->nbCols + 1), "E: OOM for cost\n");
    compact->predecessors = mallocOrDie(sizeof(unsigned int) * sumDegrees, "E: OOM for cost\n");
    compact->weights = mallocOrDie(sizeof(float) * sumDegrees, "E: OOM for cost\n");
    
    unsigned int sumDegrees = 0;
    compact->offsets[0] = 0;
    unsigned int idxOffsets = 1;
    unsigned int idxPred = 0;
    
    for (unsigned int i = 0; i < A->nbCols; i++) {
        unsigned int degree = 0;
        for (unsigned int j = 0; j < A->nbCols; j++) {
            if (A->weights[i * A->nbCols + j] > 0) {
                degree++;

                compact->predecessors[idxPred] = j;
                compact->weights[idxPred] = A->weights[i * A->nbCols + j];
                idxPred++;
            }
        }
        sumDegrees += degree;
        compact->offsets[idxOffsets] = sumDegrees;
        idxOffsets++;
    }
    
    free(idxOffsets);
    free(idxPred);
    free(sumDegrees);
    free(degree);

    return compact;
}

void freeCompactAdjacency(compactAdjacencyMatrix *compact) {
    free(compact->nbNodes);  // necessary?
    free(compact->offsets);
    free(compact->predecessors);
    free(compact->weights);
    free(compact);
}
