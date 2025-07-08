#ifndef _PATHCOUNTSWITHPREDECESSORS_H_
#define _PATHCOUNTSWITHPREDECESSORS_H_

#include "denseAdjacency.h"

/*
    this module defines the matrix with path counts for predecessors B_k
*/

/*
    pathCounts
*/
typedef struct {
    unsigned int 

} pathCounts;

pathCounts *dense2pathCounts(denseAdjacency dense*);

#endif
