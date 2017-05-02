//
// Perceptron Reuse Predictor - Implementation File
//

#include <assert.h>
#include <cmath>
#include "percepTable.h"

percepTable::percepTable( int tableSize, int maxCtr, int threshold) :
    // Set predictor properties
    tableSz(tableSize),
    maxCounter(maxCtr),
    thres(threshold)
{
    // Allocate memory for the table
    predictionTable = new int[tableSize];

    for(int i = 0; i < tableSz; i++) {
        predictionTable[i] = 0;
    }
}

void percepTable::train (int hashVal, int prediction, bool zeroReuse) {
    if(prediction != -1 && zeroReuse == true && predictionTable[hashVal] != (0 - maxCounter)) {
        predictionTable[hashVal]--;
    }
    else if(prediction != 1 && zeroReuse == false && predictionTable[hashVal] != maxCounter) {
        predictionTable[hashVal]++;
    }
}

int percepTable::reusePredict ( int hashVal ) {
    int sum = 0;
    sum += predictionTable[hashVal];

    if (sum > thres) {
        return 1;
    }
    else if (sum < (0 - thres)) {
        return -1;
    }
    else {
        return 0;
    }
    return 0;
}

int percepTable::getTableSz() {
    return tableSz;
}

int percepTable::getMaxCtr() {
    return maxCounter;
}

int percepTable::getThreshold() {
    return thres;
}