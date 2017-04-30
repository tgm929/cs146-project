
//
// L2 Data Cache
//

#ifndef __L_2_CACHE__
#define __L_2_CACHE__

#include "cache.h"

class l2cache : public cache {
public:
    l2cache(int blockSize, int totalCacheSize, int associativity, cache *nextLevel) :
        cache( blockSize, totalCacheSize, associativity, nextLevel, true)
    { }
};

#endif
