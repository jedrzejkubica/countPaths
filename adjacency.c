#include <stdio.h>
#include <string.h>

#include "adjacency.h"
#include "mem.h"

void removeLoops(adjacencyMatrix *A) {
    for (unsigned int i = 0; i < A->nbCols; i++) {
            A->weights[i * A->nbCols + i] = 0;
    }
}

/*
    parse a SIF file with the network,
    each line must be: node1, weight, node2,
    build an adjacency matrix with the weights;
    return a pointer to a freshly allocated adjacencyMatrix structure
*/
// adjacencyMatrix *readSIF(char *fileName) {
//     FILE *file = fopen(fileName, "r");
//     if (!file) {
//         fprintf(stderr, "E: Could not open file %s\n", fileName);
//         return NULL;
//     }

//     adjacencyMatrix *A = mallocOrDie(sizeof(adjacencyMatrix), "E: OOM for adjacency matrix\n");
//     A->nbCols = 0;
//     A->weights = NULL;

//     int maxLineLength = 256;
//     char *line = mallocOrDie(maxLineLength, "E: OOM for line\n");
//     unsigned int maxNode = 0;

//     while (fgets(line, sizeof(line), file)) {
//         while (line[strlen(line)-1] != '\n') {
//             // line wasn't finished? or end of file
//             int oldLineLength = maxLineLength;
//             maxLineLength *= 2;            
//             line = realloc(line, maxLineLength);
//             if (line == NULL) {
//                 fprintf(stderr, "E: OOM for extending line\n");
//                 exit(1);
//             }
//             if (fgets(line + oldLineLength - 1, oldLineLength + 1, file) == NULL) {
//                 // we hit EOF (or there was an error? don't know how to differentiate... pretend no errors)
//                 // assuming EOF: append newline so final line seems \n-terminated like it should be
//                 line + strlen(line) + 1 = '\0';
//                 line + strlen(line) = '\n';
//                 break;
//             }
//         }

//         // below code is useless, we need a hash or similar to map identifiers to ints, check
//         // if an id was seen before or not, etc...
//         unsigned int node1, node2;
//         float weight;
//         if (sscanf(line, "%u %f %u", &node1, &weight, &node2) != 3) {
//             fprintf(stderr, "E: Invalid line format in SIF file: %s\n", line);
//             continue;
//         }
//         if (node1 > maxNode) maxNode = node1;
//         if (node2 > maxNode) maxNode = node2;
//     }

//     A->nbCols = maxNode + 1; // nodes are indexed from 0 to maxNode
//     A->weights = mallocOrDie(sizeof(float) * A->nbCols * A->nbCols, "E: OOM for adjacency weights\n");

//     // Reset file pointer to the beginning
//     rewind(file);

//     // Initialize weights to 0
//     for (unsigned int i = 0; i < A->nbCols * A->nbCols; i++) {
//         A->weights[i] = 0;
//     }

//     // Read again and fill the weights
//     while (fgets(line, sizeof(line), file)) {
//         unsigned int node1, node2;
//         float weight;
//         if (sscanf(line, "%u %f %u", &node1, &weight, &node2) == 3) {
//             A->weights[node1 * A->nbCols + node2] = weight;
//         }
//     }

//     fclose(file);
    
//     return A;
// }

void freeAdjacency(adjacencyMatrix *A) {
    free(A->weights);
    free(A);
}

void printAdjacency(adjacencyMatrix *A) {
    for (unsigned int i = 0; i < A->nbCols; i++) {
        for (unsigned int j = 0; j < A->nbCols; j++) {
            printf("%0.2f ", A->weights[i * A->nbCols + j]);
        }
        printf("\n");
    }
}
