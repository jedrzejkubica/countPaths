#ifndef _PATHCOUNTS_H_
#define _PATHCOUNTS_H_

#include "pathCountsWithPredecessors.h"

/*
    pathCounts[i*nbCols + j] contains the number of paths for some length
    (or the sum of path weights for a weighted network) between nodes i and j
*/
typedef struct {
    unsigned int nbCols;
    float *pathCounts;
} pathCountsMatrix;

/*  
    sum = 0;
    for (unsigned int k = compact->offsets[j]; k < compact->offsets[j + 1]; k++)
        sum += pathCountsWithPred->data[i*offsets[nbNodes] + k]
    pathCounts[i*nbCols + j] = sum;
*/
pathCountsMatrix *countPaths(pathCountsWithPredMatrix *pathCountsWithPred, compactAdjacencyMatrix *compact);

void freePathCounts(pathCountsMatrix *countPaths);

#endif
