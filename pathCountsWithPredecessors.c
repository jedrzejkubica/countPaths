#include "pathCountsWithPredecessors.h"
#include "compactAdjacency.h"
#include "adjacency.h"
#include "mem.h"

/*
    build pathCountsWithPredMatrix for paths of length 1,
    return a pointer to a freshly allocated structure;
    
    data[i*offsets[nbNodes] + offsets[j] + k] = weights[offsets[j] + k] if:
    - node i is a predecessor of node j
    - predecessors[offsets[j] + k] == i
    or 0 otherwise
*/
pathCountsWithPredMatrix *compact2pathCounts(compactAdjacencyMatrix *compact) {
    pathCountsWithPredMatrix *pathCounts = mallocOrDie(sizeof(pathCountsWithPredMatrix), "E: OOM for path counts\n");
    
    unsigned int nbNodes = compact->nbNodes;
    unsigned int sumDegrees = compact->offsets[nbNodes];
    
    pathCounts->data = mallocOrDie(sizeof(float) * sumDegrees * nbNodes, "E: OOM for path counts data\n");
    // set to 0.0 float (all-zeroes is not guaranteed to be 0.0)
    for (size_t i = 0; i < sumDegrees * nbNodes; i++)
        pathCounts->data[i] = 0.0;

    // if i->j is an edge of weight w, then there is a path from i to j
    // with penultimate node i and weight w
    for (size_t idxOffset = 0; idxOffset < sumDegrees; idxOffset++)
	pathCounts->data[compact->predecessors[idxOffset] * sumDegrees + idxOffset] = compact->weights[idxOffset];
    return pathCounts;
}

/*
    build a pathCountsWithPredMatrix for paths of length k+1
    given pathCountsWithPredMatrix for paths of length k,
    return a pointer to a freshly allocated structure;
*/
pathCountsWithPredMatrix *buildNextPathCounts(pathCountsWithPredMatrix *pathCounts, compactAdjacencyMatrix *compact) {
    unsigned int nbNodes = compact->nbNodes;
    unsigned int sumDegrees = compact->offsets[nbNodes];
    
    pathCountsWithPredMatrix *nextPathCounts = mallocOrDie(sizeof(pathCountsWithPredMatrix), "E: OOM for next path counts\n");
    nextPathCounts->data = mallocOrDie(sizeof(float) * sumDegrees * nbNodes, "E: OOM for next path counts data\n");
    
    for (unsigned int i = 0; i < nbNodes; i++) {
        for (unsigned int j = 0; j < nbNodes; j++) {
            float sum = 0;
            for (unsigned int k = compact->offsets[j]; k < compact->offsets[j + 1]; k++) {
                // skip if i is a predecessor of j
                if (compact->predecessors[k] == i) {
                    continue;
                }

                // sum the paths that end at predecessors[k]
                if (compact->predecessors[k] != j) {
                    sum += pathCounts->data[i * sumDegrees + k];
                }
            }
            nextPathCounts->data[i * sumDegrees + k] = sum;
        }
    }
    
    return nextPathCounts;
}

void freePathCountsWithPred(pathCountsWithPredMatrix *pathCounts) {
    free(pathCounts->data);
    free(pathCounts);
}
