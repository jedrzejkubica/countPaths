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

#ifndef _BFWALK_H_
#define _BFWALK_H_

#include "network.h"
#include "scores.h"

// define DEBUG for additional debugging logs
// #define DEBUG

/*
  bfwalk() is the only symbol we want to export in the shared library
  -> we will compile with -fvisibility=hidden and change to "default" here
*/
#pragma GCC visibility push(default)

/*
    Given a network N and some seed nodes (eg known causal genes),
    apply the BFWalk algorithm to calculate a score for each node in
    the network.
    In "causal", each score must be in [0,1] and most should be 0.
    "alpha" is the BFWalk parameter, it must be in ]0,1[, typically 0.5 is good.
    "scores" must be allocated and will be filled in-place.
    If cacheFile is NULL it is ignored, otherwise it should be a filename (with path,
    path must exist):
    - if cacheFile doesn't exist it will be created and populated with data that depends
      only on N and alpha;
    - if it exists, it's content MUST match N and alpha (otherwise we log & exit),
      and it will greatly speed up bfwalk().
*/
void bfwalk(network *N, geneScores *causal, float alpha, geneScores *scores, char *cacheFile);

#endif
