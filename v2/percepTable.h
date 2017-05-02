//
// Perceptron Reuse Predictor - Header File
//

#ifndef __PERCEPT_TABLE__
#define __PERCEPT_TABLE__

#include <iostream>
#include "pin.H"

class percepTable {
public:
	percepTable ( int tableSize, int maxCtr, int threshold);

	void train (int hashIndex, int prediction, bool zeroReuse);
	int reusePredict ( int hashIndex );
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