/*
  Copyright (C) Jędrzej Kubica, Nicolas Thierry-Mieg, 2025

  This file was written by Jędrzej Kubica and Nicolas Thierry-Mieg
  (CNRS, France) Nicolas.Thierry-Mieg@univ-grenoble-alpes.fr

  This program is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with this program.
  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>

#include "network.h"
#include "mem.h"


/*
  compare two edges so they can be sorted by increasing dest then
  increasing source
 */
int compEdges(const void *elem1, const void *elem2) {
    int retVal = 0;
    edge *edge1 = (edge*) elem1;
    edge *edge2 = (edge*) elem2;

    if (edge1->dest < edge2->dest)
        retVal = -1;
    else if (edge1->dest > edge2->dest)
        retVal = 1;
    else if (edge1->source < edge2->source)
        retVal = -1;
    else if (edge1->source > edge2->source)
        retVal = 1;
    return(retVal);
}

long int checkNetwork(network *N) {
    long int nbZeroWeightEdges = 0;

    edge *edgeP = N->edges;
    for (size_t i = 0; i < N->nbEdges; i++) {
        // check if weights are in [0, 1]
        if ((edgeP->weight < 0) || (edgeP->weight > 1)) {
            // immediately return, we can't do anything with this network
            return(-1);
        }
        else if (edgeP->weight == 0)
            nbZeroWeightEdges++;
        else if (edgeP->source == edgeP->dest) {
            // self-loop
            edgeP->weight = 0;
            nbZeroWeightEdges++;
        }
        edgeP++;
    }
    // sort the edges in-place
    qsort(N->edges, N->nbEdges, sizeof(edge), compEdges);
    // make sure we don't have the same edge twice
    edgeP = N->edges;
    for (size_t i = 1; i < N->nbEdges; i++) {
        if ((edgeP->dest == (edgeP+1)->dest) && (edgeP->source == (edgeP+1)->source))
            // duplicate edge
            return(-1);
        edgeP++;
    }
    return(nbZeroWeightEdges);
}

void freeNetwork(network *N) {
    free(N->edges);
    free(N);
}

void printNetwork(network *N) {
    printf("Network has %lu nodes and %lu edges:\n", N->nbNodes, N->nbEdges);
    edge *edgeP = N->edges;
    for (size_t i = 0; i < N->nbEdges; i++) {
        printf("%u -> %u w=%0.2f\n", edgeP->source, edgeP->dest, edgeP->weight);
        edgeP++;
    }
}
