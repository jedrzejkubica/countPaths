#ifndef _ADJACENCY_H_
#define _ADJACENCY_H_

/*
    this module defines data structure and methods for adjacency matrix calculations
*/

/*
    weights[i*nbCols + j] contains the weight from node i to node j
    note: weights must be 0/1 for unweighted, or in [0, 1] for weighted
*/
typedef struct {
    unsigned int nbCols;
    float *weights;
} adjacencyMatrix;

/*
    removes edges that connect a node to itself;
    modifies A in-place
*/
void removeLoops(adjacencyMatrix *A);

/*
    parse a SIF file with the network,
    each line must be: node1, weight, node2
*/
adjacencyMatrix *readSIF(char *fileName);

void freeAdjacency(adjacencyMatrix *A);

void printAdjacency(adjacencyMatrix *A);

#endif
