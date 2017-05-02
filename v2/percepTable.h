//
// Perceptron Reuse Predictor - Header File
//

#ifndef __PERCEPT_TABLE__
#define __PERCEPT_TABLE__

#include <iostream>
#include "pin.H"

/*struct tableEntry
{
    int feature1;
};*/

class percepTable {
public:
	percepTable ( int tableSize, int maxCtr, int threshold);

	void train (unsigned long ins_ptr, int prediction, bool zeroReuse);
	int reusePredict ( unsigned long ins_ptr );
	int getTableSz();
	int getMaxCtr();
	int getThreshold();

	const int tableSz;
    const int maxCounter;
    const int thres;

    // The actual table
    int* predictionTable;
};

#endif