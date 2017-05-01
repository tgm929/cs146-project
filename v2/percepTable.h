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

class percepTable
{
public:
	virtual ~percepTable() {};

	virtual void train (int hashIndex, int prediction, bool zeroReuse);
	virtual void incrCtr ( int hashIndex );
	virtual void decrCtr ( int hashIndex );

	virtual int reusePredict ( int hashIndex );

	virtual int getTableSz();
	virtual int getMaxCtr();
	virtual int getThreshold();

protected:
	percepTable ( int tableSize, int maxCtr, int threshold);
}