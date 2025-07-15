#ifndef _TEST_ADJACENCY_H_
#define _TEST_ADJACENCY_H_

#include "adjacency.h"
#include "mem.h"

/*
    this module creates a test adjacency matrix
*/

adjacencyMatrix *createTestAdjacency(unsigned int nbCols, float minWeight, float maxWeight);

#endif