#include "pathCountsWithPredecessors.h"

/*
    the same structure as adjacency.h;

    elements[i*nbCols + j] contains the number of paths from node i to node j
*/
typedef struct {
    unsigned int nbCols;
    float *elements;
} pathCountMatrix;

/*
    sum of pathCountsWithPred[] from to is the number of paths between nodes i and j
*/
pathCountMatrix *countPaths(pathCountsWithPredMatrix *pathCountsWithPred);