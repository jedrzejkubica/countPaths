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
#include <zlib.h>

#include "cacheMats.h"
#include "network.h"
#include "signal.h"
#include "mem.h"


/* the gzipped cache file will contain in binary form:
   CACHE_MAGIC
   CACHE_VERSION
   net->nbNodes
   net->nbEdges
   the net->edges data
   alpha
   each of the nbMat signalMatrix->data
*/
#define CACHE_MAGIC "BFWALK-C"
#define CACHE_VERSION 1.0

int saveMatInit(gzFile cacheStream, network *net, float alpha) {
    /* use a SIGNALTYPE for VERSION, so we also make sure the SIGNALTYPE representation is the
       same on the running machine and on the machine that built the cache (endianness/format). */
    SIGNALTYPE version = (SIGNALTYPE)CACHE_VERSION;
    if ((gzfwrite(CACHE_MAGIC, 1, strlen(CACHE_MAGIC), cacheStream) != strlen(CACHE_MAGIC)) ||
        (gzfwrite(&(version), sizeof(version), 1, cacheStream) != 1) ||
        (gzfwrite(&(net->nbNodes), sizeof(net->nbNodes), 1, cacheStream) != 1) ||
        (gzfwrite(&(net->nbEdges), sizeof(net->nbEdges), 1, cacheStream) != 1) ||
        (gzfwrite(net->edges, sizeof(edge), net->nbEdges, cacheStream) != net->nbEdges) ||
        (gzfwrite(&(alpha), sizeof(alpha), 1, cacheStream) != 1)) {
        fprintf(stderr, "ERROR: cannot cache network data, is your partition full?\n");
        return(-1);
    }
    return(0);
}


int saveMat(gzFile cacheStream, signalMatrix *nextMat) {
    size_t nbElem = nextMat->nbNodes * nextMat->nbNodes;
    if (gzfwrite(nextMat->data, sizeof(SIGNALTYPE), nbElem, cacheStream) != nbElem) {
        fprintf(stderr, "ERROR: cannot save next mat to cachefile, is your partition full?\n");
        return(-1);
    }
    return(0);
}


int loadMatInit(gzFile cacheStream, network *net, float alpha) {
    // init magic to CACHE_MAGIC so size is correct, we will then squash the content with fread
    char magic[] = CACHE_MAGIC;
    if ((gzfread(magic, 1, strlen(CACHE_MAGIC), cacheStream) != strlen(CACHE_MAGIC)) ||
        (memcmp(magic, CACHE_MAGIC, strlen(CACHE_MAGIC)) != 0)) {
        fprintf(stderr, "ERROR: cache magic mismatch, this doesn't look like a BFWalk cacheFile\n");
        return(-1);
    }
    SIGNALTYPE version;
    if ((gzfread(&version, sizeof(SIGNALTYPE), 1, cacheStream) != 1) || (version != (SIGNALTYPE)CACHE_VERSION)) {
        fprintf(stderr, "ERROR: cacheFile version or endianness/format mismatch, make a fresh cache\n");
        return(-1);
    }
    unsigned long int nbNodes, nbEdges;
    size_t dataRead = gzfread(&(nbNodes), sizeof(nbNodes), 1, cacheStream);
    dataRead += gzfread(&(nbEdges), sizeof(nbEdges), 1, cacheStream);
    if (dataRead != 2) {
        fprintf(stderr, "ERROR: cannot read header from provided cachefile\n");
        return(-1);
    }
    if ((nbNodes != net->nbNodes) || (nbEdges != net->nbEdges)) {
        fprintf(stderr, "ERROR: provided cachefile doesn't correspond to network (nbNodes,nbEdges)\n");
        return(-1);
    }

    edge *edges = mallocOrDie(nbEdges * sizeof(edge), "E: OOM for temp edges in loadMatInit\n");
    if (gzfread(edges, sizeof(edge), nbEdges, cacheStream) != nbEdges) {
        fprintf(stderr, "ERROR: cannot read edges from provided cachefile\n");
        free(edges);
        return(-1);
    }
    if (memcmp(edges, net->edges, sizeof(edge) * nbEdges) != 0) {
        fprintf(stderr, "ERROR: provided cachefile doesn't correspond to network (edges)\n");
        free(edges);
        return(-1);
    }
    // OK we don't need edges anymore
    free(edges);
    
    float alphaFromStream;
    if ((gzfread(&(alphaFromStream), sizeof(alphaFromStream), 1, cacheStream) != 1) ||
        (alphaFromStream != alpha)) {
        fprintf(stderr, "ERROR: provided cachefile alpha can't be read or differs from specified alpha\n");
        return(-1);
    }

    // if we got here: AOK, cache matches network
    return(0);
}


signalMatrix *loadNextMat(gzFile cacheStream, size_t nbNodes) {
    // is cacheStream empty?
    int nextC = gzgetc(cacheStream);
    if (nextC == EOF)
        return(NULL);
    else
        gzungetc(nextC, cacheStream);

    signalMatrix *nextMat = mallocOrDie(sizeof(signalMatrix), "OOM for signalMatrix in loadNextMat\n");
    nextMat->nbNodes = nbNodes;
    size_t nbElem = nbNodes * nbNodes;
    nextMat->data = mallocOrDie(sizeof(SIGNALTYPE) * nbElem, "E: OOM for signalMatrix data\n");
    if (gzfread(nextMat->data, sizeof(SIGNALTYPE), nbElem, cacheStream) != nbElem) {
        fprintf(stderr, "ERROR: cannot load next mat from cachefile\n");
        exit(1);
    }
    return(nextMat);
}

    
