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
#include <zlib.h>

#include "gbaCentrality.h"
#include "network.h"
#include "cacheMats.h"
#include "compactAdjacency.h"
#include "normFactor.h"
#include "signal.h"
#include "scores.h"
#include "mem.h"


/*********************************************************************
  Declare private functions (documented when defined at bottom of file)
*********************************************************************/
static double calculateNorm(signalMatrix *sumOfSignal);
static void updateScores(geneScores *scores, geneScores *causal, signalMatrix *signalMat);
static int gbaCentralityFromCache(network *N, geneScores *causal, float alpha, geneScores *scores, char *cacheFile);
static void gbaCentralityNoCache(network *N, geneScores *causal, float alpha, geneScores *scores, char *cacheFile);


/*********************************************************************
  Define public function
*********************************************************************/

void gbaCentrality(network *N, geneScores *causal, float alpha, geneScores *scores, char *cacheFile) {
    // sanity check:
    if (N->nbNodes != causal->nbGenes) {
        fprintf(stderr, "ERROR: gbaCentrality() called with network and causal genes of different sizes\n");
        exit(1);
    }

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

    int done = 0;
    if (cacheFile) {
        FILE *cacheStream = fopen(cacheFile, "r");
        if (cacheStream) {
            // cacheFile exists...
            fclose(cacheStream);
            if (gbaCentralityFromCache(N, causal, alpha, scores, cacheFile))
                // cacheFile doesn't correspond to N or alpha
                exit(1);
            // all good, scores filled from cacheFile
            done = 1;
        }
    }
    if (done == 0)
        // cacheFile is NULL or doesn't exist, calculate GBA matrices
        gbaCentralityNoCache(N, causal, alpha, scores, cacheFile);
}


/*********************************************************************
  Define private functions
*********************************************************************/

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

/*
  Fill scores with GBA-centrality, using GBA matrices previously
  calculated and stored in cacheFile (must exist).
  Pre-condition: checkNetwork(N) was called beforehand.
  Return 0 if AOK, -1 if cache doesn't correspond to the network N or alpha.
  Die on errors (OOM or cache broken).
*/
static int gbaCentralityFromCache(network *N, geneScores *causal, float alpha, geneScores *scores, char *cacheFile) {
    size_t nbGenes = causal->nbGenes;
    gzFile cacheStream = gzopen(cacheFile, "r");
    if (cacheStream == NULL) {
        fprintf(stderr, "ERROR: gbaCentralityFromCache() called but cacheFile can't be gz-opened\n");
        exit(1);
    }
    if (loadMatInit(cacheStream, N, alpha) == -1) {
        fprintf(stderr, "ERROR: gbaCentrality() called with mismatched network and cacheFile\n");
        fprintf(stderr, "provide a non-existing cacheFile to create a cache from the current network\n");
        gzclose(cacheStream);
        return(-1);
    }

    // start by copying causal scores, ie scores = alpha**0 * causal * I
    memcpy(scores->scores, causal->scores, nbGenes * sizeof(SCORETYPE));

    signalMatrix *sumOfSignal = loadNextMat(cacheStream, N->nbNodes);
    while (sumOfSignal != NULL) {
        updateScores(scores, causal, sumOfSignal);
        freeSignal(sumOfSignal);
        sumOfSignal = loadNextMat(cacheStream, N->nbNodes);
    }
    gzclose(cacheStream);
    return(0);
}


/*
  Fill scores with GBA-centrality, calculating GBA matrices and
  saving them to cacheFile if non-NULL.
  Pre-condition: checkNetwork(N) was called beforehand.
  On errors (eg OOM or issue saving to cacheFile), remove (incomplete)
  cacheFile and exit(1).
*/
static void gbaCentralityNoCache(network *N, geneScores *causal, float alpha, geneScores *scores, char *cacheFile) {
    size_t nbGenes = causal->nbGenes;
    gzFile cacheStream = NULL;
    if (cacheFile) {
        FILE *fileExists = fopen(cacheFile, "r");
        if (fileExists) {
            fprintf(stderr, "ERROR: gbaCentralityNoCache() called but cacheFile exists\n");
            fclose(fileExists);
            exit(1);
        }
        cacheStream = gzopen(cacheFile, "w");
        if (cacheStream == NULL) {
            fprintf(stderr, "ERROR: gbaCentralityNoCache() called but cacheFile can't be created\n");
            fprintf(stderr, "The path must exist, does it? And do you have write permissions there?\n");
            exit(1);
        }
        if (saveMatInit(cacheStream, N, alpha) == -1) {
            fprintf(stderr, "ERROR: gbaCentralityNoCache() called to build cacheFile but saveMatInit() failed\n");
            gzclose(cacheStream);
            remove(cacheFile);
            exit(1);
        }
    }
    
    // start by copying causal scores, ie scores = alpha**0 * causal * I
    memcpy(scores->scores, causal->scores, nbGenes * sizeof(SCORETYPE));

    // for convergence test
    double threshold = 1E-4;
    size_t k = 1;

    compactAdjacencyMatrix *networkComp = network2compact(N);
    // calculate normalization factors (used in each iteration)
    normFactorVector *normFactVec = buildNormFactorVector(networkComp, alpha);
    
    #ifdef DEBUG
    fprintf(stderr, "INFO gbaCentrality(): calculating M~_%ld\n", k);
    #endif
    signalWithPredMatrix *signalCurrent = buildFirstSignal(networkComp, normFactVec);
    signalMatrix *sumOfSignal = signalSum(signalCurrent, networkComp);
    
    while (calculateNorm(sumOfSignal) > threshold) {
        // update scores with effect of causal genes at distance K
        updateScores(scores, causal, sumOfSignal);
        // save M~_k matrix to cache if requested
        if ((cacheStream) && (saveMat(cacheStream, sumOfSignal) == -1)) {
            fprintf(stderr, "ERROR: gbaCentrality() called to build cacheFile but saveMat() failed\n");
            gzclose(cacheStream);
            remove(cacheFile);
            exit(1);
        }
        
        // build M~_(k+1) for next iteration
        #ifdef DEBUG
        fprintf(stderr, "INFO gbaCentrality(): calculating M~_%ld\n", k+1);
        #endif
        signalWithPredMatrix *signalNext = buildNextSignal(signalCurrent, sumOfSignal, networkComp, normFactVec);
        freeSignalWithPred(signalCurrent);
        signalCurrent = signalNext;
        freeSignal(sumOfSignal);
        sumOfSignal = signalSum(signalCurrent, networkComp);
        k++;
    }

    // clean up
    freeSignalWithPred(signalCurrent);
    freeSignal(sumOfSignal);
    freeCompactAdjacency(networkComp);
    freeNormFactorVector(normFactVec);
    if (cacheStream)
        gzclose(cacheStream);
}
