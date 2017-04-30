//
// L1 Instruction Cache
//

#ifndef __L_1_I_CACHE__
#define __L_1_I_CACHE__

#include "cache.h"

class l1icache : public cache {
public:
    l1icache( int blockSize, int totalCacheSize, int associativity, cache *nextLevel) :
        cache( blockSize, totalCacheSize, associativity, nextLevel, false)
    { }
};

#endif
