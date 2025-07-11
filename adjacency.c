#include "adjacency.h"

void removeLoops(adjacencyMatrix *A) {
    for (unsigned int i = 0; i < A->nbCols; i++) {
        for (unsigned int j = 0; j < A->nbCols; j++) {
            if (i == j) {
                A->weights[i * A->nbCols + j] = 0;
            }
        }
    }
}

adjacencyMatrix *readSIF(char *fileName) {
    // TODO
}

void freeAdjacency(adjacencyMatrix *A) {
    free(A->nbCols)  // necessary?
    free(A->weights);
    free(A);
}

void printAdjacency(adjacencyMatrix *A) {
    for (unsigned int i = 0; i < A->nbCols; i++) {
        for (unsigned int j = 0; j < A->nbCols; j++) {
            float el = A->weights[i * A->nbCols + j]
            printf("%f ", el);
        }
        printf("\n");
    }
}
