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
#include <math.h>

#include "network.h"
#include "scores.h"
#include "gbaCentrality.h"
#include "mem.h"

// for comparing obtained scores with constant floats
#define EPSILON 1e-6

/* weighted direct network: B <- A -> C, weights 0.5 and 1, A is the seed */
network *threeWeighted(void) {
    network *threeWeighted = mallocOrDie(sizeof(network), "E: OOM for threeWeighted");
    threeWeighted->nbNodes = 3;
    threeWeighted->nbEdges = 2;
    threeWeighted->edges = mallocOrDie(threeWeighted->nbEdges * sizeof(edge), "E: OOM for threeWeighted edges");
    edge *edgeP = threeWeighted->edges;

    edgeP->source = 0;
    edgeP->dest = 1;
    edgeP->weight = 0.5;
    edgeP++;
    edgeP->source = 0;
    edgeP->dest = 2;
    edgeP->weight = 1;
    edgeP++;
    return(threeWeighted);
}
geneScores *causalGenesThreeWeighted(void) {
    geneScores *causal = mallocOrDie(sizeof(geneScores), "E: OOM for causal genes");
    causal->nbGenes = 3;
    causal->scores = mallocOrDie(causal->nbGenes * sizeof(SCORETYPE), "E: OOM for causal genes");

    for (size_t i = 0; i < causal->nbGenes; i++)
        causal->scores[i] = 0;
    causal->scores[0] = 1;
    return(causal);
}

/* weighted direct network: A-> B, A->C, B->D, C->D, weights 0.1 for A->B and 1 for all others,
   one self-interaction D->D (should get removed), A is the seed */
network *diamond4(void) {
    network *diamond4 = mallocOrDie(sizeof(network), "E: OOM for diamond4");
    diamond4->nbNodes = 4;
    diamond4->nbEdges = 5;
    diamond4->edges = mallocOrDie(diamond4->nbEdges * sizeof(edge), "E: OOM for diamond4 edges");
    edge *edgeP = diamond4->edges;

    edgeP->source = 0;
    edgeP->dest = 1;
    edgeP->weight = 0.1;
    edgeP++;
    edgeP->source = 0;
    edgeP->dest = 2;
    edgeP->weight = 1;
    edgeP++;

    edgeP->source = 1;
    edgeP->dest = 3;
    edgeP->weight = 1;
    edgeP++;

    edgeP->source = 2;
    edgeP->dest = 3;
    edgeP->weight = 1;
    edgeP++;
    
    // self-interaction 3->3 for testing that it is removed
    edgeP->source = 3;
    edgeP->dest = 3;
    edgeP->weight = 1;
    return(diamond4);
}
geneScores *causalGenesDiam4(void) {
    geneScores *causal = mallocOrDie(sizeof(geneScores), "E: OOM for causal genes");
    causal->nbGenes = 4;
    causal->scores = mallocOrDie(causal->nbGenes * sizeof(SCORETYPE), "E: OOM for causal genes");

    for (size_t i = 0; i < causal->nbGenes; i++)
        causal->scores[i] = 0;
    causal->scores[0] = 1;
    return(causal);
}


/* TODO document this diamond6 test: what network, what seeds?
   All we know for now is: "node 1 without edges and node 2 with only a self-loop"
*/
network *diamond6(void) {
    network *diamond6 = mallocOrDie(sizeof(network), "E: OOM for diamond6");
    diamond6->nbNodes = 6;
    diamond6->nbEdges = 10;
    diamond6->edges = mallocOrDie(diamond6->nbEdges * sizeof(edge), "E: OOM for diamond6 edges");
    edge *edgeP = diamond6->edges;

    edgeP->source = 0;
    edgeP->dest = 3;
    edgeP->weight = 0.2;
    edgeP++;
    edgeP->source = 0;
    edgeP->dest = 4;
    edgeP->weight = 0.5;
    edgeP++;

    edgeP->source = 3;
    edgeP->dest = 0;
    edgeP->weight = 0.4;
    edgeP++;
    edgeP->source = 3;
    edgeP->dest = 5;
    edgeP->weight = 1;
    edgeP++;

    edgeP->source = 4;
    edgeP->dest = 0;
    edgeP->weight = 1;
    edgeP++;
    edgeP->source = 4;
    edgeP->dest = 5;
    edgeP->weight = 1;
    edgeP++;

    edgeP->source = 5;
    edgeP->dest = 3;
    edgeP->weight = 1;
    edgeP++;
    edgeP->source = 5;
    edgeP->dest = 4;
    edgeP->weight = 1;
    edgeP++;
    
    // add self-interactions 2->2 and 3->3
    edgeP->source = 2;
    edgeP->dest = 2;
    edgeP->weight = 1;
    edgeP++;
    edgeP->source = 3;
    edgeP->dest = 3;
    edgeP->weight = 1;
    edgeP++;

    return(diamond6);
}
geneScores *causalGenesDiam6(void) {
    geneScores *causal = mallocOrDie(sizeof(geneScores), "E: OOM for causal genes");
    causal->nbGenes = 6;
    causal->scores = mallocOrDie(causal->nbGenes * sizeof(SCORETYPE), "E: OOM for causal genes");

    for (size_t i = 0; i < causal->nbGenes; i++)
        causal->scores[i] = 0;
    causal->scores[1] = 1;
    causal->scores[4] = 1;
    return(causal);
}


/*
  7 nodes: node 1 is connected to nodes 0 and 2, node 2 is a hub connected to 1 and 3-6,
  and we create this as an undirected unweighted network. Node 1 will be the only seed.
*/
network *asymmetric(void) {
    network *asym = mallocOrDie(sizeof(network), "E: OOM for asym");
    asym->nbNodes = 7;
    asym->nbEdges = 12;
    asym->edges = mallocOrDie(asym->nbEdges * sizeof(edge), "E: OOM for asym edges");
    edge *edgeP = asym->edges;

    edgeP->source = 0;
    edgeP->dest = 1;
    edgeP->weight = 1;
    edgeP++;
    
    edgeP->source = 1;
    edgeP->dest = 0;
    edgeP->weight = 1;
    edgeP++;
    edgeP->source = 1;
    edgeP->dest = 2;
    edgeP->weight = 1;
    edgeP++;

    edgeP->source = 2;
    edgeP->dest = 1;
    edgeP->weight = 1;
    edgeP++;
    edgeP->source = 2;
    edgeP->dest = 3;
    edgeP->weight = 1;
    edgeP++;
    edgeP->source = 2;
    edgeP->dest = 4;
    edgeP->weight = 1;
    edgeP++;
    edgeP->source = 2;
    edgeP->dest = 5;
    edgeP->weight = 1;
    edgeP++;
    edgeP->source = 2;
    edgeP->dest = 6;
    edgeP->weight = 1;
    edgeP++;

    edgeP->source = 3;
    edgeP->dest = 2;
    edgeP->weight = 1;
    edgeP++;
    edgeP->source = 4;
    edgeP->dest = 2;
    edgeP->weight = 1;
    edgeP++;
    edgeP->source = 5;
    edgeP->dest = 2;
    edgeP->weight = 1;
    edgeP++;
    edgeP->source = 6;
    edgeP->dest = 2;
    edgeP->weight = 1;
    
    return(asym);
}
geneScores *causalGenesAsym(void) {
    geneScores *causal = mallocOrDie(sizeof(geneScores), "E: OOM for causal genes");
    causal->nbGenes = 7;
    causal->scores = mallocOrDie(causal->nbGenes * sizeof(SCORETYPE), "E: OOM for causal genes");

    for (size_t i = 0; i < causal->nbGenes; i++)
        causal->scores[i] = 0;
    causal->scores[1] = 1;
    return(causal);
}

int main(void) {
    printf("\n#############################################################################\n");
    {
        // threeWeighted
        network *threeW = threeWeighted();
        printf("threeWeighted network\n");
        printNetwork(threeW);

        geneScores *causal = causalGenesThreeWeighted();
        printf("threeWeighted causal genes:\n");
        printScores(causal);
        
        geneScores *result = mallocOrDie(sizeof(geneScores), "E: OOM for result scores");
        result->nbGenes = causal->nbGenes;
        result->scores = mallocOrDie(causal->nbGenes * sizeof(SCORETYPE), "E: OOM for result scores");

        gbaCentrality(threeW, causal, 0.5, result, NULL);
        printf("threeWeighted scores\n");
        printScores(result);
        // compare with expected final scores
        if ((result->scores[0] == 1) && (result->scores[1] == 0.25) && (result->scores[2] == 0.5))
            printf("AOK, SCORES ARE AS EXPECTED\n");
        else
            printf("ERROR: SCORES FOR threeWeighted network ARE WRONG!\n");
        freeNetwork(threeW);
        freeScores(causal);
        freeScores(result);
    }
    printf("\n#############################################################################\n");
    {
        // dimaond4 network
        network *diam4 = diamond4();
        printf("diam4 with its self-loop\n");
        printNetwork(diam4);
    
        geneScores *causal = causalGenesDiam4();
        printf("Diam4 causal genes:\n");
        printScores(causal);

        geneScores *result = mallocOrDie(sizeof(geneScores), "E: OOM for result scores");
        result->nbGenes = causal->nbGenes;
        result->scores = mallocOrDie(causal->nbGenes * sizeof(SCORETYPE), "E: OOM for result scores");

        gbaCentrality(diam4, causal, 0.5, result, NULL);
        printf("Diam4 scores\n");
        printScores(result);
        // compare with expected final scores
        if ((result->scores[0] == 1) && (fabsf(result->scores[1] - 0.05f) < EPSILON) &&
            (result->scores[2] == 0.5) && (fabsf(result->scores[3] - 0.1375f) < EPSILON))
            printf("AOK, SCORES ARE AS EXPECTED\n");
        else
            printf("ERROR: SCORES FOR diam4 network ARE WRONG!\n");
        freeNetwork(diam4);
        freeScores(causal);
        freeScores(result);
    }
    printf("\n#############################################################################\n");
    {
        // dimaond6 network
        network *diam6 = diamond6();
        printf("diam6 with its self-loop\n");
        printNetwork(diam6);
    
        geneScores *causal = causalGenesDiam6();
        printf("Diam6 causal genes:\n");
        printScores(causal);

        geneScores *result = mallocOrDie(sizeof(geneScores), "E: OOM for result scores");
        result->nbGenes = causal->nbGenes;
        result->scores = mallocOrDie(causal->nbGenes * sizeof(SCORETYPE), "E: OOM for result scores");

        gbaCentrality(diam6, causal, 0.5, result, NULL);
        printf("diam6 scores\n");
        // TODO compare with expected
        printScores(result);
        printf("DUNNO WHAT TO EXPECT, FIXME!\n");
        freeNetwork(diam6);
        freeScores(causal);
        freeScores(result);
    }
    printf("\n#############################################################################\n");
    {
        // asymmetric network, creating a cache file
        network *asym = asymmetric();
        printf("asymmetric with one hub, creating cache file on the fly\n");
        printNetwork(asym);
    
        geneScores *causal = causalGenesAsym();
        printf("ASYM causal genes:\n");
        printScores(causal);

        geneScores *result = mallocOrDie(sizeof(geneScores), "E: OOM for result scores");
        result->nbGenes = causal->nbGenes;
        result->scores = mallocOrDie(causal->nbGenes * sizeof(SCORETYPE), "E: OOM for result scores");

        gbaCentrality(asym, causal, 0.5, result, "cachefile_GBA_asym");
        printf("ASYM scores\n");
        printScores(result);
        // compare with expected:
        if ((result->scores[0] == 0.5) && (result->scores[1] == 1) && (fabsf(result->scores[2] - 0.1f) < EPSILON) &&
            (fabsf(result->scores[3] - 0.05f) < EPSILON) && (fabsf(result->scores[4] - 0.05f) < EPSILON) &&
            (fabsf(result->scores[5] - 0.05f) < EPSILON) && (fabsf(result->scores[6] - 0.05f) < EPSILON))
            printf("AOK, SCORES ARE AS EXPECTED\n");
        else
            printf("ERROR: SCORES FOR ASYM network ARE WRONG!\n");
        freeNetwork(asym);
        freeScores(causal);
        freeScores(result);
    }
    printf("\n#############################################################################\n");
    {
        // asymmetric network, using the cache file
        network *asym = asymmetric();
        printf("asymmetric with one hub, using the cache file we created\n");
        printNetwork(asym);
    
        geneScores *causal = causalGenesAsym();
        printf("ASYM causal genes:\n");
        printScores(causal);

        geneScores *result = mallocOrDie(sizeof(geneScores), "E: OOM for result scores");
        result->nbGenes = causal->nbGenes;
        result->scores = mallocOrDie(causal->nbGenes * sizeof(SCORETYPE), "E: OOM for result scores");

        gbaCentrality(asym, causal, 0.5, result, "cachefile_GBA_asym");
        printf("ASYM scores (using cached matrices)\n");
        printScores(result);
        // compare with expected:
        if ((result->scores[0] == 0.5) && (result->scores[1] == 1) && (fabsf(result->scores[2] - 0.1f) < EPSILON) &&
            (fabsf(result->scores[3] - 0.05f) < EPSILON) && (fabsf(result->scores[4] - 0.05f) < EPSILON) &&
            (fabsf(result->scores[5] - 0.05f) < EPSILON) && (fabsf(result->scores[6] - 0.05f) < EPSILON))
            printf("AOK, SCORES ARE AS EXPECTED\n");
        else
            printf("ERROR: SCORES FOR ASYM network USING CACHE ARE WRONG!\n");
        freeNetwork(asym);
        freeScores(causal);
        freeScores(result);
        // clean up cachefile
        remove("cachefile_GBA_asym");
    }
    printf("\n#############################################################################\n");

    return(0);
}

