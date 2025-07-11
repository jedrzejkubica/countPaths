#ifndef _PATHCOUNTSWITHPREDECESSORS_H_
#define _PATHCOUNTSWITHPREDECESSORS_H_

#include "compactAdjacency.h"

/*
    this module defines the matrix of path counts with predecessors
*/

/*
    this structure is dependent on a compactAdjacencyMatrix;

    data is of size offsets[nbNodes]*nbNodes;

    for any k in [0, deg(j)-1]:
    data[i*offsets[nbNodes] + offsets[j] + k]
    is the number of paths (or the sum of path weights for a weighted network)
    between nodes i and j whose penultimate node is predecessors[offsets[j] + k]
*/
typedef struct {
    float *data;
} pathCountsWithPredMatrix;

/*
    build pathCountsWithPredMatrix for paths of length 1,
    return a pointer to a freshly allocated structure;
*/
pathCountsWithPredMatrix *compact2pathCounts(compactAdjacencyMatrix *compact);

/*
    build a pathCountsWithPredMatrix for paths of length k+1
    given pathCountsWithPredMatrix for paths of length k,
    return a pointer to a freshly allocated structure;
*/
pathCountsWithPredMatrix *buildNextPathCounts(pathCountsWithPredMatrix *pathCounts, compactAdjacencyMatrix *compact);

void freePathCountsWithPred(pathCountsWithPredMatrix *pathCounts);

#endif
