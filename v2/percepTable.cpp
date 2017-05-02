//
// Perceptron Reuse Predictor - Implementation File
//

#include <assert.h>
#include <cmath>
#include "percepTable.h"
#include "stdio.h"

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

void percepTable::train (unsigned long ins_ptr, int prediction, bool zeroReuse) {
    int hashVal = (ins_ptr >> 6) % getTableSz();

    //printf("hashVal = %d\n", hashVal);
    prediction++;

    if(zeroReuse == true && predictionTable[hashVal] != (0 - maxCounter)) {
        predictionTable[hashVal]--;
    }
    else if(zeroReuse == false && predictionTable[hashVal] != maxCounter) {
        predictionTable[hashVal]++;
    }
}

int percepTable::reusePredict ( unsigned long ins_ptr ) {
    //printf("test 2a\n");
    int hashVal = (ins_ptr >> 6) % getTableSz();
    //printf("hashVal = %d\n", hashVal);
    int sum = 0;
    sum += predictionTable[hashVal];
    //printf("test 2a\n");

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