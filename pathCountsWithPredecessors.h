#ifndef _PATHCOUNTSWITHPREDECESSORS_H_
#define _PATHCOUNTSWITHPREDECESSORS_H_

#include "denseAdjacency.h"

/*
    this module defines the matrix with path counts for predecessors B_k
*/

/*
    predecessorsMatrix
*/
typedef struct {
    unsigned int 

} predecessorsMatrix;

predecessorsMatrix *dense2predecessorsMatrix(denseAdjacency dense*);

#endif
