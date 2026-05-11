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

#include "normFactor.h"
#include "compactAdjacency.h"
#include "mem.h"


/*
  Build and return the normFactorVector corresponding to the provided compact
*/
normFactorVector *buildNormFactorVector(compactAdjacencyMatrix *compact, float alpha) {
    size_t nbNodes = compact->nbNodes;
    size_t sumDegrees = compact->offsets[nbNodes];

    normFactorVector *normFactVec = mallocOrDie(sizeof(normFactorVector), "E: OOM for normFactVector\n");
    normFactVec->data = mallocOrDie(sizeof(float) * sumDegrees, "E: OOM for normFact\n");

    // set to 0.0 (all-zeroes is not guaranteed to be 0.0)
    for (size_t i = 0; i < sumDegrees; i++)
        normFactVec->data[i] = 0;

    for (size_t j = 0; j < nbNodes; j++) {
        double sumOfWeights = 0; // higher precision (double) for running sum
        for (size_t offset = compact->offsets[j]; offset < compact->offsets[j + 1]; offset++)
            sumOfWeights += compact->weights[offset];
        // convert to float for normFactVec but make sure it fits
        float sumAsFloat = (float)sumOfWeights;
        if (isinf(sumAsFloat)) {
            fprintf(stderr, "ERROR: sum of weights incoming to node %lu too large\n", j);
            exit(1);
        }
        if (sumAsFloat != 0)
            for (size_t offset = compact->offsets[j]; offset < compact->offsets[j + 1]; offset++)
                // i is implicit here, it is compact->predecessors[offset]
                normFactVec->data[offset] = alpha * compact->weights[offset] * compact->weights[offset] / sumAsFloat;
        // else it stays zero
    }
    return(normFactVec);
}


void freeNormFactorVector(normFactorVector *normFactVec) {
    free(normFactVec->data);
    free(normFactVec);
}
