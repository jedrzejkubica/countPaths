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
    offsets[j] is the first index corresponding to node j in successors and weights,
    offsets[nbNodes] is the sum of degrees, used to avoid overflowing;

    edges outgoing from j go to nodes successors[offsets[j]] up to successors[offsets[j+1]-1];
    the corresponding edge weights (outgoing from j) are weights[offsets[j]] up to weights[offsets[j+1]-1].
*/
typedef struct {
    unsigned int nbNodes;
    size_t *offsets;
    unsigned int *successors;
    float *weights;
} compactAdjacencyMatrix;

compactAdjacencyMatrix *adjacency2compact(adjacencyMatrix *A);

void freeCompactAdjacency(compactAdjacencyMatrix *compactA);

#endif
