#include <assert.h>
#include "victim.h"

victim_cache::victim_cache( int blockSize, int totalCacheSize) :
    cache( blockSize, totalCacheSize, totalCacheSize / blockSize, nullptr, false, 0)
    { }

bool victim_cache::check_hit( unsigned long address) {
    // your code here	
    return true;
}

void victim_cache::toDcache(unsigned long address) {
    // your code here

}

void victim_cache::fromDcache( unsigned long address) {
    // your code here	
}
