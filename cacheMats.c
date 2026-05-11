/*
  Copyright (C) Jędrzej Kubica, Nicolas Thierry-Mieg, 2026

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

#include "cacheMats.h"
#include "network.h"
#include "signal.h"
#include "mem.h"


/* the cache file will contain in binary form:
   nbMat (number of matrices stored in file)
   net->nbNodes
   net->nbEdges
   the net->edges data
   alpha
   each of the nbMat signalMatrix->data
*/

void saveMatInit(FILE *cacheStream, network *net, float alpha) {
    // we don't yet know how many matrices there will be
    int nbMat = 0;
    if ((fwrite(&(nbMat), sizeof(nbMat), 1, cacheStream) != 1) ||
        (fwrite(&(net->nbNodes), sizeof(net->nbNodes), 1, cacheStream) != 1) ||
        (fwrite(&(net->nbEdges), sizeof(net->nbEdges), 1, cacheStream) != 1) ||
        (fwrite(net->edges, sizeof(edge), net->nbEdges, cacheStream) != net->nbEdges) ||
        (fwrite(&(alpha), sizeof(alpha), 1, cacheStream) != 1)) {
        fprintf(stderr, "ERROR: cannot cache network data, is your partition full?\n");
        exit(1);
    }
}


void saveMat(FILE *cacheStream, signalMatrix *nextMat) {
    size_t nbElem = nextMat->nbNodes * nextMat->nbNodes;
    if (fwrite(nextMat->data, sizeof(SIGNALTYPE), nbElem, cacheStream) != nbElem) {
        fprintf(stderr, "ERROR: cannot save next mat to cachefile, is your partition full?\n");
        exit(1);
    }
    // increment nbMat
    long currentPos = ftell(cacheStream);
    rewind(cacheStream);
    int nbMat;
    fread(&(nbMat), sizeof(nbMat), 1, cacheStream);
    nbMat++;
    rewind(cacheStream);
    fwrite(&(nbMat), sizeof(nbMat), 1, cacheStream);
    fseek(cacheStream, currentPos, SEEK_SET);
}


int loadMatInit(FILE *cacheStream, network *net, float alpha) {
    int nbMat;
    size_t dataRead = fread(&(nbMat), sizeof(nbMat), 1, cacheStream);
    unsigned long int nbNodes, nbEdges;
    dataRead += fread(&(nbNodes), sizeof(nbNodes), 1, cacheStream);
    dataRead += fread(&(nbEdges), sizeof(nbEdges), 1, cacheStream);
    if (dataRead != 3) {
        fprintf(stderr, "ERROR: cannot read header from provided cachefile\n");
        return(-1);
    }
    if ((nbNodes != net->nbNodes) || (nbEdges != net->nbEdges)) {
        fprintf(stderr, "ERROR: provided cachefile doesn't correspond to network (nbNodes,nbEdges)\n");
        return(-1);
    }

    edge *edges = mallocOrDie(nbEdges * sizeof(edge), "E: OOM for temp edges in loadMatInit\n");
    if (fread(edges, sizeof(edge), nbEdges, cacheStream) != nbEdges) {
        fprintf(stderr, "ERROR: cannot read edges from provided cachefile\n");
        return(-1);
    }
    if (memcmp(edges, net->edges, sizeof(edge) * nbEdges) != 0) {
        fprintf(stderr, "ERROR: provided cachefile doesn't correspond to network (edges)\n");
        return(-1);
    }
    // OK we don't need edges anymore
    free(edges);
    
    float alphaFromStream;
    if ((fread(&(alphaFromStream), sizeof(alphaFromStream), 1, cacheStream) != 1) ||
        (alphaFromStream != alpha)) {
        fprintf(stderr, "ERROR: provided cachefile alpha can't be read or differs from specified alpha\n");
        return(-1);
    }

    // if we got here: AOK, cache matches network
    return(nbMat);
}


/*
  Return a signalMatrix (allocated here) containing the next matrix stored in cacheStream.
    After the call, cacheStream points to the next signalMatrix->data (if any).
*/
signalMatrix *loadNextMat(FILE *cacheStream, size_t nbNodes) {
    signalMatrix *nextMat = mallocOrDie(sizeof(signalMatrix), "OOM for signalMatrix in loadNextMat\n");
    nextMat->nbNodes = nbNodes;
    size_t nbElem = nbNodes * nbNodes;
    nextMat->data = mallocOrDie(sizeof(SIGNALTYPE) * nbElem, "E: OOM for signalMatrix data\n");
    if (fread(nextMat->data, sizeof(SIGNALTYPE), nbElem, cacheStream) != nbElem) {
        fprintf(stderr, "ERROR: cannot load next mat from cachefile\n");
        exit(1);
    }
    return(nextMat);
}

    
