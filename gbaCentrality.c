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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#include "gbaCentrality.h"
#include "network.h"
#include "compactAdjacency.h"
#include "normFactor.h"
#include "signal.h"
#include "scores.h"
#include "mem.h"


/*
  Private function: return a norm of the sumOfsignal matrix
*/
static double calculateNorm(signalMatrix *sumOfSignal);

void gbaCentrality(network *N, geneScores *causal, float alpha, geneScores *scores) {
    // sanity check:
    if (N->nbNodes != causal->nbGenes) {
        fprintf(stderr, "ERROR: gbaCentrality() called with network and causal genes of different sizes");
        exit(1);
    }
    size_t nbGenes = causal->nbGenes;

    // start by copying causal scores, ie scores = alpha**0 * causal * I
    memcpy(scores->scores, causal->scores, nbGenes * sizeof(SCORETYPE));

    compactAdjacencyMatrix *networkComp = network2compact(N);

    // calculate normalization factors (used in each iteration)

    #ifdef DEBUG
        fprintf(stderr, "INFO gbaCentrality(): calculating normalization factors\n");
    #endif
    normFactorVector *normFactVec = buildNormFactorVector(networkComp, alpha);
    
    #ifdef DEBUG
        fprintf(stderr, "INFO gbaCentrality(): calculating B_1\n");
    #endif
    signalWithPredMatrix *signalCurrent = buildFirstSignal(networkComp, normFactVec);
    signalMatrix *sumOfSignal = signalSum(signalCurrent, networkComp);

    double normOfMat = calculateNorm(sumOfSignal);
    #ifdef DEBUG
        fprintf(stderr, "INFO gbaCentrality(): normOfMat = %f\n", normOfMat);
    #endif

    size_t k = 1;
    // for convergence test
    double threshold = 1E-4;

    while (normOfMat > threshold) {
        // update scores with effect of causal genes at distance K: scores += causal * B_k
        #pragma omp parallel for
        for (size_t j = 0; j < nbGenes; j++) {
            for (size_t i = 0; i < nbGenes; i++) {
                scores->scores[j] += causal->scores[i] * sumOfSignal->data[i* nbGenes + j];
            }
        }

        // build B_(k+1) for next iteration

        #ifdef DEBUG
            fprintf(stderr, "INFO gbaCentrality(): calculating B_%ld\n", k+1);
        #endif
        signalWithPredMatrix *signalNext = buildNextSignal(signalCurrent, sumOfSignal, networkComp, normFactVec);

        freeSignalWithPred(signalCurrent);
        signalCurrent = signalNext;
        freeSignal(sumOfSignal);
        sumOfSignal = signalSum(signalCurrent, networkComp);
        normOfMat = calculateNorm(sumOfSignal);
        #ifdef DEBUG
            fprintf(stderr, "INFO gbaCentrality(): normOfMat = %f\n", normOfMat);
        #endif
        k++;
    }
    freeNormFactorVector(normFactVec);
    freeSignalWithPred(signalCurrent);
    freeSignal(sumOfSignal);
    freeCompactAdjacency(networkComp);
}

/*
  Return Frobenius norm of sumOfSignal
*/
static double calculateNorm(signalMatrix *sumOfSignal) {
    double frobNorm = 0; // double == high precision for the running sum
    for (size_t i = 0; i < sumOfSignal->nbNodes * sumOfSignal->nbNodes; i++) {
        frobNorm += sumOfSignal->data[i] * sumOfSignal->data[i];
    }
    frobNorm = sqrt(frobNorm);
    return(frobNorm);
}
