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
#include "cacheMats.h"
#include "compactAdjacency.h"
#include "normFactor.h"
#include "signal.h"
#include "scores.h"
#include "mem.h"


/*
  Private function: return a norm of the sumOfsignal matrix
*/
static double calculateNorm(signalMatrix *sumOfSignal);
/*
  Private function: update scores with effect of causal genes via signalMat
*/
static void updateScores(geneScores *scores, geneScores *causal, signalMatrix *signalMat);



void gbaCentrality(network *N, geneScores *causal, float alpha, geneScores *scores, char *cacheFile) {
    // sanity check:
    if (N->nbNodes != causal->nbGenes) {
        fprintf(stderr, "ERROR: gbaCentrality() called with network and causal genes of different sizes\n");
        exit(1);
    }
    size_t nbGenes = causal->nbGenes;

    // check network, set self-loops to zero-weight, sort by dest then source
    long int nbZeroWeightEdges = checkNetwork(N);
    if (nbZeroWeightEdges == -1) {
        fprintf(stderr, "E: weights are not in [0, 1], please fix the network\n");
        exit(1);
    } else if (nbZeroWeightEdges == 1) {
        fprintf(stderr, "INFO: your network has 1 self-loop or zero-weight edge, it will be ignored\n");
    } else if (nbZeroWeightEdges > 1) {
        fprintf(stderr, "INFO: your network has %li self-loops and/or zero-weight edges, they will be ignored\n",
                nbZeroWeightEdges);
    }


    FILE *cacheStream = NULL;
    // cacheMode: 0 if no cachefile, 1 if using an existing cachefile, 2 if creating a new file
    int cacheMode = 0;
    // number of remaining matrices in cache (only meaningful if cacheMode==1)
    int nbMat = 0;
    if (cacheFile) {
        cacheStream = fopen(cacheFile, "r");
        if (cacheStream) {
            cacheMode = 1;
            nbMat = loadMatInit(cacheStream, N, alpha);
            if (nbMat == -1) {
                fprintf(stderr, "ERROR: gbaCentrality() called with mismatched network and cacheFile\n");
                fprintf(stderr, "provide a non-existing cacheFile to create a cache from the current network\n");
                exit(1);
            }
        }
        else {
            cacheMode = 2;
            // need write AND read access for saveMat()
            cacheStream = fopen(cacheFile, "w+");
            if (cacheStream == NULL) {
                fprintf(stderr, "ERROR: gbaCentrality() called with cacheFile that doesn't exist and can't be created\n");
                fprintf(stderr, "The path must exist, does it? And do you have write permissions there?\n");
                exit(1);
            }
            if (saveMatInit(cacheStream, N, alpha) == -1) {
                fprintf(stderr, "ERROR: gbaCentrality() called requesting creation of cacheFile but saveMatInit() failed\n");
                exit(1);
            }
        }
    }
    
    // start by copying causal scores, ie scores = alpha**0 * causal * I
    memcpy(scores->scores, causal->scores, nbGenes * sizeof(SCORETYPE));

    // for convergence test
    double threshold = 1E-4;
    size_t k = 1;

    // following won't be really used if cacheMode==1, but must be declared in this scope
    compactAdjacencyMatrix *networkComp = NULL;
    normFactorVector *normFactVec = NULL;
    signalWithPredMatrix *signalCurrent = NULL;
    signalMatrix *sumOfSignal = NULL;
    double normOfMat;

    if (cacheMode != 1) {
        networkComp = network2compact(N);
        // calculate normalization factors (used in each iteration)
        normFactVec = buildNormFactorVector(networkComp, alpha);
    
        #ifdef DEBUG
        fprintf(stderr, "INFO gbaCentrality(): calculating B_%ld\n", k);
        #endif
        signalCurrent = buildFirstSignal(networkComp, normFactVec);
        sumOfSignal = signalSum(signalCurrent, networkComp);

        normOfMat = calculateNorm(sumOfSignal);
        #ifdef DEBUG
        fprintf(stderr, "INFO gbaCentrality(): normOfMat = %f\n", normOfMat);
        #endif
    }
    else if (nbMat > 0) {
        sumOfSignal = loadNextMat(cacheStream, N->nbNodes);
        nbMat--;
        // we want to keep going until there are no more cached matrices
        normOfMat = threshold + 1;
    }
    else {
        // no matrices at all, strange but whatever
        normOfMat = threshold - 1;
    }
    
    while (normOfMat > threshold) {
        // update scores with effect of causal genes at distance K: scores += causal * B_k
        updateScores(scores, causal, sumOfSignal);
        // save B_k matrix to cache if requested
        if (cacheMode == 2) {
            if (saveMat(cacheStream, sumOfSignal) == -1) {
                fprintf(stderr, "ERROR: gbaCentrality() called requesting creation of cacheFile but saveMat() failed\n");
                exit(1);
            }
        }
        
        if (cacheMode != 1) {
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
        }
        else if (nbMat > 0) {
            freeSignal(sumOfSignal);
            sumOfSignal = loadNextMat(cacheStream, N->nbNodes);
            nbMat--;
            normOfMat = threshold + 1;
        }
        else {
            // cacheMode==1 but no remaining matrices
            freeSignal(sumOfSignal);
            normOfMat = threshold - 1;
        }
        k++;
    }

    // clean up
    if (cacheMode != 0)
        fclose(cacheStream);
    if (cacheMode != 1) {
        freeNormFactorVector(normFactVec);
        freeSignalWithPred(signalCurrent);
        freeSignal(sumOfSignal);
        freeCompactAdjacency(networkComp);
    }
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

/*
  Update scores with effect of causal genes via signalMat: scores += causal * signalMat
*/
static void updateScores(geneScores *scores, geneScores *causal, signalMatrix *signalMat) {
    size_t nbGenes = causal->nbGenes;
    #pragma omp parallel for
    for (size_t j = 0; j < nbGenes; j++) {
        for (size_t i = 0; i < nbGenes; i++) {
            scores->scores[j] += causal->scores[i] * signalMat->data[i* nbGenes + j];
        }
    }
}
