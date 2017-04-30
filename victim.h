//
// victim cache
//
#ifndef __VICTIM_CACHE__
#define __VICTIM_CACHE__

#include "pin.H"
#include "cache.h"

class victim_cache : public cache {
public:
    victim_cache(int blockSize, int totalCacheSize);

    bool check_hit( unsigned long address);
   	void toDcache(unsigned long address);
   	void fromDcache(unsigned long address);
   	void addressRequest(unsigned long address){};
};

#endif
