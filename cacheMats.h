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

#ifndef _CACHEMATS_H_
#define _CACHEMATS_H_

#include <stdio.h>

#include "network.h"
#include "signal.h"


/* Save or load signalMatrix matrices to/from a cachefile.
   These matrices depend only on the network and on alpha, not on the seeds (causal);
   and calculating them is the bottleneck in GBA-centrality, therefore caching them
   can greatly speed things up.
*/

/*
  Save the provided network data to cacheStream, so we can compare when loading.
  cacheStream must be open for writing.
  Return 0 if AOK, -1 if something fails (and log an error message on stderr).
*/
int saveMatInit(FILE *cacheStream, network *net, float alpha);


/*
  Save nextMat->data to cacheStream.
  cacheStream must be open for writing AND reading.
  Return 0 if AOK, -1 if something fails (and log an error message on stderr).
*/
int saveMat(FILE *cacheStream, signalMatrix *nextMat);


/*
  Load a network from cachStream and compare with net/alpha.
  Return -1 if data in cacheStream doesn't correspond to net/alpha or
  if there was an error reading cacheStream,
  the number of cached matrices otherwise.
  After the call, cacheStream points to the first signalMatrix->data.
*/
int loadMatInit(FILE *cacheStream, network *net, float alpha);


/*
  Return a signalMatrix (allocated here) containing the next matrix stored in cacheStream.
    After the call, cacheStream points to the next signalMatrix->data (if any).
*/
signalMatrix *loadNextMat(FILE *cacheStream, size_t nbNodes);

#endif
