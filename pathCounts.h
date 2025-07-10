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
    sum of pathCountsWithPred[] from to is the number of paths between nodes i and j
*/
pathCountsMatrix *countPaths(pathCountsWithPredMatrix *pathCountsWithPred, compactAdjacencyMatrix *compact);

freePathCounts();

#endif
