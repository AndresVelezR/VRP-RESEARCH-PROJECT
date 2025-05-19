#ifndef INITIAL_SEEDS_H
     #define INITIAL_SEEDS_H

     #include "Individual.h"
     #include "Params.h"

     Individual ClarkeWrightSeed(Params &params);
     Individual NearestNeighborSeed(Params &params);
     Individual RandomKeySeed(Params &params);

#endif
