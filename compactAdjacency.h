#ifndef _COMPACTADJACENCY_H_
#define _COMPACTADJACENCY_H_

#include <stddef.h>
#include "adjacency.h"

/*
    this module defines the compact representation of adjacency matrix
*/

/*
    compactAdjacencyMatrix represents an adjacency matrix of size nbNodes x nbNodes;

    offsets has nbNodes+1 elements,
    offsets[j] is the first index corresponding to node j in predecessors and weights,
    offsets[nbNodes] is the sum of degrees, used to avoid overflowing;

    nodes with an edge incoming to j are nodes predecessors[offsets[j]] up to predecessors[offsets[j+1]-1];
    the corresponding edge weights (incoming to j) are weights[offsets[j]] up to weights[offsets[j+1]-1]

    Similarly nodes with an edge outgoing from j are nodes successors[offsetsSuccessors[j]] up
    to successors[offsetsSuccessors[j+1]-1] (we don't store the weights in this direction)
*/
typedef struct {
    unsigned int nbNodes;
    size_t *offsets;
    unsigned int *predecessors;
    float *weights;
    size_t *offsetsSuccessors;
    unsigned int *successors;
} compactAdjacencyMatrix;

compactAdjacencyMatrix *adjacency2compact(adjacencyMatrix *A);

void freeCompactAdjacency(compactAdjacencyMatrix *compactA);

#endif
