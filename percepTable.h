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

	virtual void incrCtr ( int hashval );
	virtual void decrCtr ( int hashval );

	virtual int getPercepVal ( int hashval );

	virtual int getTableSz();
	virtual int getMaxCtr();

protected:
	percepTable ( int tableSize, int maxCtr);
}