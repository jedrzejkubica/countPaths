#ifndef _PATHCOUNTSWITHPREDECESSORS_H_
#define _PATHCOUNTSWITHPREDECESSORS_H_

#include "compactAdjacency.h"

/*
    this module defines the matrix of path counts with predecessors
*/

/*
    pathCountsWithPred is of size offsets[nbNodes] x nbNodes,
    pathCountsWithPred[i*offsets[nbNodes] + j] corresponds to the number of paths
    between nodes i and j with penultimate node p;
*/
typedef struct {
    unsigned int nbNodes;
    unsigned int *pathCountsWithPred;
} pathCountsWithPredMatrix;

/*
    initialize pathCountsWithPredMatrix;
    
    pathCountsWithPred[i*offsets[nbNodes] + j] = weights[offsets[j]] if:
    - node i is a predecessor of node j
    - node p is a predecessor of node i
    or skip otherwise?
*/
pathCountsWithPredMatrix *compact2pathCountsWithPred(unsigned int *offsets, unsigned int *predecessors, float *weights);

/*
    idx = i * offsets[nbNodes]
    sum = 0
    for (offset = offsets[p]; offset < offsets[p+1]; offset++) {
        if (predecessors[offset] != j) {
            sum += pathCountsWithPred[idx + offset];
        }
    }
*/
pathCountsWithPredMatrix *updatePathCountsWithPred(
    pathCountsWithPredMatrix *pathCountsWithPred, 
    unsigned int *offsets,
    unsigned int *predecessors,
    float *weights
);

void freePathCountsWithPred(pathCountsWithPredMatrix *pathCountsWithPred);

#endif
