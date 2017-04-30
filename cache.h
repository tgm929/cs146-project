//
// Generic Cache - Header File
//

#ifndef __GEN_CACHE__
#define __GEN_CACHE__

#include <iostream>
#include "pin.H"

using namespace std;

// Number of bits in the address
const static UINT64 ADDRESS_SIZE = 64;

// Defines the Structure of a Cache Entry
// LRU_status stores the adges of a cache line
// if LRU_status == 0, the cache line is Most Recently Used.
// else if LRU_status == assoc - 1, the cache line is one of the oldest cache lines.
struct cacheEntry
{
    int LRU_status;
    unsigned long Tag;
    bool Valid;
    bool zeroReuse;
    int insertion;
};

class cache
{
public:
    virtual ~cache() {};

    // Access the generic cache.
    // You might want to override this in L1D to access the victim cache.
    virtual void addressRequest( unsigned long address);

    // Get Statistics Methods
    virtual UINT64 getTotalMiss();
    virtual UINT64 getHit();
    virtual UINT64 getRequest();
    virtual UINT64 getEntryRemoved();

    // Get Settings Methods
    virtual int getCacheSize();
    virtual int getCacheAssoc();
    virtual int getCacheBlockSize();
    virtual unsigned int getTagSize() {return tagSize;}
    virtual unsigned int getBlockOffsetSize() {return blockOffsetSize;}
    virtual unsigned int getSetSize() {return setSize;}

protected:
    cache( int blockSize, int totalCacheSize, int associativity, cache* nextLevel, bool writebackDirty );

    //Calculate the Tag and Set of an address based on this cache's properties
    unsigned int getTag( unsigned int address );
    unsigned int getSet( unsigned int address );

    // returns -1 for a miss
    // index into array for a hit
    int isHit( unsigned int tagBits, unsigned int setBits );

    // Set a certain index as the Most Receintly Used in its
    // associativity, and adjusts all the appropriate indices to match
    // the LRU scheme
    void updateLRU( int setBits, int MRU_index );

    void queueInsert( int setBits, int MRU_index, int insertion_pos );

    // Gets the index of the LRU index for a given set of setBits
    int getLRU( int setBits );

    // Gets the index of the MRU index for a given set of setBits
    int getMRU( int setBits );

    // Initializes this cache
    void clearCache();

    void addTotalMiss();
    void addHit();
    void addRequest();
    void addEntryRemoved();

    // Given Properties
    const int blockSz;
    const int totalCacheSz;
    const int assoc;

    // Bit Field Sizes
    const unsigned int blockOffsetSize;
    const unsigned int setSize;
    const unsigned int tagSize;

    const unsigned int tagMask;
    const unsigned int setMask;
    const int maxSetValue;

    // Statistics
    UINT64 totalMisses;
    UINT64 hits;
    UINT64 requests;
    UINT64 entriesKickedOut;
    UINT64 zeroReuseEvictions;

    // The actual cache array
    cacheEntry* cacheMem;

    // The next level in the cache hierachy
    cache* const nextLevel;
    // Does this cache write evicted items to the next level (icaches don't need to)
    const bool writebackDirty;
};

#endif
