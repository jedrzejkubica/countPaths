#ifndef _TEST_ADJACENCY_H_
#define _TEST_ADJACENCY_H_

#include "testAdjacency.h"
#include "adjacency.h"
#include "mem.h"

adjacencyMatrix *createTestAdjacency(unsigned int nbCols, float minWeight, float maxWeight) {
    adjacencyMatrix *A = mallocOrDie(sizeof(adjacencyMatrix), "E: OOM for test adjacency matrix\n");
    A->nbCols = nbCols;
    A->weights = mallocOrDie(nbCols * nbCols * sizeof(float), "E: OOM for test adjacency weights\n");

    for (unsigned int i = 0; i < nbCols; i++)
        for (unsigned int j = 0; j < nbCols; j++)
                A->weights[i * nbCols + j] = minWeight + (float)rand() / (float)(RAND_MAX / (maxWeight - minWeight));
    
    return A;
}