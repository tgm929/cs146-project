
//
// L2 Data Cache
//

#ifndef __L_2_CACHE__
#define __L_2_CACHE__

#include <assert.h>
#include "cache.h"


class l2cache : public cache {
public:
    l2cache(int blockSize, int totalCacheSize, int associativity, cache *nextLevel, int insertion_pos) :
        cache( blockSize, totalCacheSize, associativity, nextLevel, true, insertion_pos)
    { }

    // Indicate that a zero-reuse block was evicted
	void addZeroReuse()
	{
	    zeroReuseEvictions++;
	}

	UINT64 getZeroReuses()
	{
		return zeroReuseEvictions;
	}

	void queueInsert( int setBits, int MRU_index, int insert_pos )
	{
	    int upperBounds = assoc - 1;

	    // Update all of the other blocks behind the newly inserted block
	    for( int i = 0; i < assoc; i++ )
	    {
	        if( cacheMem[ i + setBits*assoc ].LRU_status >= insert_pos &&
	                cacheMem[ i + setBits*assoc ].LRU_status < upperBounds )
	        {
	            cacheMem[ i + setBits*assoc ].LRU_status++;
	        }
	    }    

	    // set the LRU position of the new block to the desired value
	    cacheMem[ MRU_index + setBits*assoc ].LRU_status = insert_pos;
	}

	void addressRequest( unsigned long address ) {

    // Compute Set / Tag
    unsigned long tagField = getTag( address );
    unsigned long setField = getSet( address );

    // Hit or Miss ?
    int index = isHit( tagField, setField );

    // Count that access
    addRequest();

    // Miss
    if( index == -1 ) {
        // Get the LRU index
        int indexLRU = getLRU( setField );
        if( cacheMem[ indexLRU + setField*assoc].Valid == true ) {
            // check if the block was ever reused
            if (cacheMem [indexLRU + setField*assoc].zeroReuse == true) {
                addZeroReuse ();
            }
            addEntryRemoved();
        }

        // Count that miss
        addTotalMiss();

        assert(nextLevel != nullptr);
        // Write the evicted entry to the next level
        if( writebackDirty &&
            cacheMem[ indexLRU + setField*assoc].Valid == true) {
            int tag = cacheMem[indexLRU + setField*assoc].Tag;
            tag = tag << (getSetSize() + getBlockOffsetSize());
            int Set = setField;
            Set = Set << (getBlockOffsetSize());
            int lru_addr = tag + Set;
            nextLevel->addressRequest(lru_addr);
        }
        // Load the requested address from next level
        nextLevel->addressRequest(address);

        // Update LRU / Tag / Valid
        cacheMem[ indexLRU + setField*assoc].Tag = tagField;
        cacheMem[ indexLRU + setField*assoc].Valid = true;
        cacheMem[ indexLRU + setField*assoc].Valid = true;
        cacheMem[ indexLRU + setField*assoc].zeroReuse = true;        
        // Insert it at a specified value within the queue
        queueInsert(setField, indexLRU, insertPos);
    }
    else {
        // Count that hit
        addHit();

        // Update LRU / Tag / Valid
        updateLRU( setField, index );

        // set mark line as used at least once
        cacheMem[ index + setField*assoc].zeroReuse = false;                

    }
}
};

#endif
