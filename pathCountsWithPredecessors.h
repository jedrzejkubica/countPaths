#ifndef _PATHCOUNTSWITHPREDECESSORS_H_
#define _PATHCOUNTSWITHPREDECESSORS_H_

#include "compactAdjacency.h"

/*
    this module defines the matrix with path counts for predecessors
*/

/*
    pathCounts[p] = number of paths 
*/
typedef struct {
    unsigned int nbRows;
    unsigned int *pathCounts;
    
} pathCountsMatrix;

/*
    initialize pathCountsMatrix
*/
void initializePathCounts(pathCountsMatrix *pathCounts);


/*
    get sum_p'_not_j(B_k[i, j][p'])
    [33, 17, 20, 55, ...]
    degreesSum[i] == sum of degrees of node i
*/
unsigned int *degreesSum(denseAdjacencyMatrix *denseAdjacency);

/*
    get A[p, j]
*/
float *getWeight(float *weights, unsigned int p, unsigned int j);

/*
    recursively update path counts
*/
void updatePathCounts(unsigned int *degreesSum, float *getWeight(float *weights));

#endif
