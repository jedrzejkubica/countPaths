#ifndef _PATHCOUNTS_H_
#define _PATHCOUNTS_H_

#include "pathCountsWithPredecessors.h"
#include "compactAdjacency.h"


/*
    data[i*nbCols + j] contains the number of paths
    (or the sum of path weights for a weighted network)
    of a given length from node i to j
*/
typedef struct {
    unsigned int nbCols;
    float *data;
} pathCountsMatrix;

/*  
    produce pathCountsMatrix corresponding to pathCountsWithPredMatrix
*/
pathCountsMatrix *countPaths(pathCountsWithPredMatrix *pathCountsWithPred, compactAdjacencyMatrix *compact);

void printPathCounts(pathCountsMatrix *pathCounts);

void freePathCounts(pathCountsMatrix *pathCounts);

#endif
