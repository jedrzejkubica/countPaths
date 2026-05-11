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
#include <math.h>
#include <omp.h>

#include "signal.h"
#include "compactAdjacency.h"
#include "normFactor.h"
#include "mem.h"


signalWithPredMatrix *buildFirstSignal(compactAdjacencyMatrix *compact, normFactorVector *normFactVec) {
    size_t nbNodes = compact->nbNodes;
    size_t sumDegrees = compact->offsets[nbNodes];

    signalWithPredMatrix *signal = mallocOrDie(sizeof(signalWithPredMatrix), "E: OOM for signalWithPred\n");
    signal->data = mallocOrDie(sizeof(SIGNALTYPE) * sumDegrees * nbNodes, "E: OOM for signalWithPred data\n");
    
    
    // set to 0.0 (all-zeroes is not guaranteed to be 0.0)
    for (size_t i = 0; i < sumDegrees * nbNodes; i++)
        signal->data[i] = 0;
    
    // if i->j is an edge, the signal is already calculated: just use normFactVec
    for (size_t offset = 0; offset < sumDegrees; offset++)
        signal->data[compact->predecessors[offset] * sumDegrees + offset] = (SIGNALTYPE)normFactVec->data[offset];
    return(signal);
}

signalWithPredMatrix *buildNextSignal(signalWithPredMatrix *signalWithPred, signalMatrix *signal,
                                      compactAdjacencyMatrix *compact, normFactorVector *normFactVec) {
    size_t nbNodes = compact->nbNodes;
    size_t sumDegrees = compact->offsets[nbNodes];
    
    signalWithPredMatrix *nextSignal = mallocOrDie(sizeof(signalWithPredMatrix), "E: OOM for next signalWithPred\n");
    nextSignal->data = mallocOrDie(sizeof(SIGNALTYPE) * sumDegrees * nbNodes, "E: OOM for next signalWithPred data\n");
    
    /*
      Ideally we want to propagate along paths, ie walks that don't contain loops. But
      this is hard to code efficiently...
      We therefore have an approximate solution, that propagates along all walks except
      those that:
      - contain a loop (of any length) back to the starting node (with i!=j below);
      - contain a loop of length 2, ie with 2 steps back-and-forth, anywhere in
        the walk (using offsetReverseEdge below).
      Therefore we are still propagating along walks that contain an inner loop of
      length >= 3  (eg i->j->k->m->j will be counted as a length 4 path between i and j)...
      But our approximate solution is efficient, and should largely reduce the
      "hub" issue, ie the explosion of number of walks when going through a hub.
    */
    #pragma omp parallel for
    for (size_t i = 0; i < nbNodes; i++) {
        for (size_t j = 0; j < nbNodes; j++) {
            for (size_t offset = compact->offsets[j]; offset < compact->offsets[j + 1]; offset++) {
                SIGNALTYPE sum = 0;
                // ignore loops back to starting node => signal stays zero if i==j
                if (i != j) {
                    size_t k = compact->predecessors[offset];
                    sum = signal->data[i * nbNodes + k];
                    // ignore walks whose last step is backtracking on the previous step
                    if (compact->offsetsReverseEdge[offset] < sumDegrees) {
                        sum -= signalWithPred->data[i * sumDegrees + compact->offsetsReverseEdge[offset]];
                    }
                    sum *= normFactVec->data[offset];
                }
                nextSignal->data[i * sumDegrees + offset] = sum;
            }
        }
    }
    return(nextSignal);
}

void freeSignalWithPred(signalWithPredMatrix *signal) {
    free(signal->data);
    free(signal);
}

signalMatrix *signalSum(signalWithPredMatrix *signalWithPred, compactAdjacencyMatrix *compact) {
    size_t nbNodes = compact->nbNodes;
    size_t sumDegrees = compact->offsets[nbNodes];

    signalMatrix *signal = mallocOrDie(sizeof(signalMatrix), "E: OOM for signalSum matrix\n");
    signal->nbNodes = nbNodes;
    signal->data = mallocOrDie(sizeof(SIGNALTYPE) * nbNodes * nbNodes, "E: OOM for signalSum data\n");
    
    #pragma omp parallel for
    for (size_t i = 0; i < nbNodes; i++) {
        for (size_t j = 0; j < nbNodes; j++) {
            double sum = 0;
            for (size_t k = compact->offsets[j]; k < compact->offsets[j + 1]; k++)
                sum += signalWithPred->data[i * sumDegrees + k];

            if (isinf(sum)) {
                fprintf(stderr, "ERROR: sum of signal from node %lu to node %lu too large\n", i, j);
                exit(1);
            }
            signal->data[i * nbNodes + j] = (SIGNALTYPE)sum;
        }
    }
    
    return(signal);
}

void printSignal(signalMatrix *signal) {
    for (size_t i = 0; i < signal->nbNodes; i++) {
        for (size_t j = 0; j < signal->nbNodes; j++)
            printf("%0.2f ", (float)signal->data[i * signal->nbNodes + j]);

        printf("\n");
    }
}

void freeSignal(signalMatrix *signal) {
    free(signal->data);
    free(signal);
}
