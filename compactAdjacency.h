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

#ifndef _COMPACTADJACENCY_H_
#define _COMPACTADJACENCY_H_

#include <stddef.h>
#include "network.h"

/*
    this module defines the compact representation of an adjacency matrix
*/

/*
    compactAdjacencyMatrix represents an adjacency matrix of size nbNodes x nbNodes;

    offsets has nbNodes+1 elements,
    offsets[j] is the first index corresponding to node j in predecessors and weights,
    offsets[nbNodes] is the sum of degrees, used to avoid overflowing;

    nodes with an edge going into node j are nodes predecessors[offsets[j]]
    up to predecessors[offsets[j+1]-1];

    the corresponding edge weights (coming into node j) are weights[offsets[j]]
    up to weights[offsets[j+1]-1]

    offsetsReverseEdge has sumOfDegrees elements (same as predecessors and weights),
    for any i < sumOfDegrees offsetsReverseEdge[i] is the offset corresponding to 
    edge j->p (assuming i is the offset of edge p->j) if j->p exists, sumOfDegrees
    otherwise

    We must use size_t for offsets, since one signalWithpredMatrix can have
    more elements than UINT_MAX. Actually we will use size_t everywhere, the
    predecessors is not large anyways.
*/
typedef struct {
    size_t nbNodes;
    size_t *offsets;
    size_t *predecessors;
    float *weights;
    size_t *offsetsReverseEdge;
} compactAdjacencyMatrix;

/*
  Allocate and populate a compactAdjacencyMatrix from a network.
  Pre-conditions: the network's edges must be sorted by increasing dest
  then increasing source, and any self-interactions must be zero-weight;
  these are true if the network went through checkNetwork()
 */
compactAdjacencyMatrix *network2compact(network *N, long int nbZeroWeight);

void freeCompactAdjacency(compactAdjacencyMatrix *compactA);

#endif
