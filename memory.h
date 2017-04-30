//
// memory
//

#ifndef __MEMORY__
#define __MEMORY__

#include "cache.h"

// We shoehorn memory in the cache interface, so we don't need to special-case
// the last-level cache as a backing store. It's a trivial cache that 'always hits'
// and only tracks the number of requests.
class memory : public cache {

public:
    memory() :
        cache(1, 1, 1, nullptr, false)
    { }

    void addressRequest( unsigned long address ) {
        (void) address;
        addRequest();
    }
};

#endif
