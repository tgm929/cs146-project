#include "pin.H"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "memory.h"
#include "L1InstCache.h"
#include "L1DataCache.h"
#include "L2Cache.h"
#include "percepTable.h"

#include "stdio.h"

using namespace std;
/*
This code implements a simple cache simulator. In this implementation, we
assume there is L1 Instruction Cache, L1 Data Cache, and L2 Cache.

We have a base class declared in cache.h file and three derived classes:
L1InstCache, L1DataCache, and L2Cache.

We have a simple memory class to track the number of memory requests.

This is a simplified cache model since we do not differentiate read and write
accesses, which is not true for real cache system.

We only track the number of hits and misses in both L1 and L2 caches.

In both L1 and L2 caches, we use a simple LRU algorithm as cache replacement policy.

*/

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,    "pintool",
        "outfile", "cache.out", "Cache results file name");

KNOB<string> KnobConfigFile(KNOB_MODE_WRITEONCE,    "pintool",
        "config", "config-base", "Configuration file name");

KNOB<UINT64> KnobInstructionCount(KNOB_MODE_WRITEONCE, "pintool",
        "max_inst","1000000000", "Number of instructions to profile");

KNOB<UINT64> KnobInsertPos(KNOB_MODE_WRITEONCE, "pintool",
        "lru_pos", "0", "Default LRU insertion position");

// Globals for the various caches
/* ===================================================================== */
l1icache* icache;
l1dcache* dcache;
l2cache* llcache;
memory* mem;
percepTable* perceptron;

// Keep track if instruction counts so we know when to end simmulation
UINT64 icount;

void PrintResults(void);

/* ===================================================================== */
INT32 Usage()
{
    cerr << "This tool represents a cache simulator.\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}


/* ===================================================================== */
void CreateCaches(void)
{
    // Check if config file exits
    ifstream config;
    config.open( KnobConfigFile.Value().c_str());
    if(!config.is_open()) {
        cerr << "Cannot open input file : " << KnobConfigFile.Value() << "\n";
        Usage();
        PIN_ExitProcess(EXIT_FAILURE);
    }

    // Create the one and only memory
    mem = new memory();

    //int ins_pos = (int) KnobInsertPos.Value();

    // Parse config file and create the three caches
    int i = 0;
    while (!config.eof()){
        string line;
        getline(config, line);
        istringstream parser(line);
        int bsize, csize, assoc, vsize, tsize, maxCtr, threshold;
        char comma = ',';
        switch(i){
            case 0:
                parser >> bsize >> comma >> csize >> comma >> assoc;
                llcache = new l2cache(bsize, csize, assoc, mem);
                break;
            case 1:
                parser >> bsize >> comma >> csize >> comma >> assoc;
                icache = new l1icache(bsize, csize, assoc, llcache);
                break;
            case 2:
                parser >> bsize >> comma >> csize >> comma >> assoc >> comma >> vsize;
                dcache = new l1dcache(bsize, csize, assoc, llcache);
                break;
            case 3:
                parser >> tsize >> comma >> maxCtr >> comma >> threshold;
                perceptron = new percepTable(tsize, maxCtr, threshold);
                break;
            default:
                break;
        }
        i++;
    }
}

/* ===================================================================== */
void CheckInstructionLimits(void)
{
    if (KnobInstructionCount.Value() > 0 &&
        icount > KnobInstructionCount.Value()) {
        PrintResults();
        PIN_ExitProcess(EXIT_SUCCESS);
    }
}

/* ===================================================================== */
void MemoryOp(ADDRINT address, ADDRINT ins_ptr)
{
    //printf("MemoryOp\n");

    if(dcache->checkHit(address))
    {
        //printf("dcache hit\n");
        dcache->addressHit(address);
    }
    else
    {
        //printf("dcache miss\n");
        if(llcache->checkHit(address))
        {
            //printf("llcache hit\n");
            dcache->addressMiss(address);
            llcache->addressReuse(address);
        }
        else
        {
            //printf("llcache miss\n");
            // Writeback
            unsigned long victim = dcache->getVictim(address);
            if (victim != 0){
                llcache->addressHit(victim);
            }

            //printf("writeback complete\n");

            // Training
            unsigned long hash_addr = llcache->getHashIndex(address);
            //printf("writeback 1\n");
            int prediction = llcache->getPrediction(address);
            //printf("writeback 2\n");
            bool zeroReuse = llcache->getZeroReuse(address);
            //printf("writeback 3\n");
            perceptron->train(hash_addr, prediction, zeroReuse);

            //printf("training complete\n");

            // Replacement
            dcache->addressMiss(address);
            int reusePrediction = perceptron->reusePredict(ins_ptr);
            int tableSize = perceptron->getTableSz();
            llcache->addressMissDynamic(address, reusePrediction, ins_ptr, tableSize);
            mem->addressRequest(address);
            mem->addressRequest(address);

            //printf("llcache replacement\n");
        }
    }
}

/* ===================================================================== */
void AllInstructions(ADDRINT ins_ptr)
{
    //printf("AllInstructions\n");

    icount++;

    if(icache->checkHit(ins_ptr))
    {
        //printf("icache hit\n");
        icache->addressHit(ins_ptr);
    }
    else
    {
        //printf("icache miss\n");
        if(llcache->checkHit(ins_ptr))
        {
            //printf("llcache hit\n");
            icache->addressMiss(ins_ptr);
            llcache->addressReuse(ins_ptr);
        }
        else
        {
            //printf("llcache miss\n");
            // Training
            int hash_addr = llcache->getHashIndex(ins_ptr);
            int prediction = llcache->getPrediction(ins_ptr);
            bool zeroReuse = llcache->getZeroReuse(ins_ptr);
            perceptron->train(hash_addr, prediction, zeroReuse);

            //printf("Training complete\n");

            // Replacement
            //printf("test 1\n");
            icache->addressMiss(ins_ptr);
            //printf("test 2\n");
            int reusePrediction = perceptron->reusePredict(ins_ptr);
            //printf("test 3\n");
            int tableSize = perceptron->getTableSz();
            //printf("test 4\n");
            llcache->addressMissDynamic(ins_ptr, reusePrediction, ins_ptr, tableSize);
            //printf("test 5\n");
            mem->addressRequest(ins_ptr);
            mem->addressRequest(ins_ptr);

            //printf("llcache replacement\n");
        }
    }

    CheckInstructionLimits();
}

/* ===================================================================== */
void PrintResults(void)
{
    ofstream out(KnobOutputFile.Value().c_str());

    out.setf(ios::fixed, ios::floatfield);
    out.precision(2);

    float l2missrate =  (float)llcache->getTotalMiss() / ((float) llcache->getRequest()) * 100.;
    float l2zerreouserate =  (float)llcache->getTotalZeroReuses() / ((float) llcache->getEntryRemoved()) * 100.;


    out << "---------------------------------------";
    out << endl << "\t\tSimulation Results" << endl;
    out << "---------------------------------------";

    out << endl
        << "Memory system->" << endl
        << "\t\tDcache size (bytes)         : " << dcache->getCacheSize() << endl
        << "\t\tDcache ways                 : " << dcache->getCacheAssoc() << endl
         << "\t\tDcache block size (bytes)   : " << dcache->getCacheBlockSize() << endl;

    out << endl
        << "\t\tIcache size (bytes)         : " << icache->getCacheSize() << endl
        << "\t\tIcache ways                 : " << icache->getCacheAssoc() << endl
        << "\t\tIcache block size (bytes)   : " << icache->getCacheBlockSize() << endl;

    out << endl
        << "\t\tL2-cache size (bytes)       : " << llcache->getCacheSize() << endl
        << "\t\tL2-cache ways               : " << llcache->getCacheAssoc() << endl
        << "\t\tL2-cache block size (bytes) : " << llcache->getCacheBlockSize() << endl;

    out << endl
        << "\t\tPerceptron Table size (bytes)       : " << perceptron->getTableSz() << endl
        << "\t\tPerceptron Table Max Counter Value  : " << perceptron->getMaxCtr() << endl
        << "\t\tPerceptron Table Threshold Value    : " << perceptron->getThreshold() << endl;

    out << "Simulated events->" << endl;

    out << "\t\t I-Cache Miss: " << icache->getTotalMiss() << " out of " << icache->getRequest() << endl;

    out << "\t\t D-Cache Miss: " << dcache->getTotalMiss() << " out of " << dcache->getRequest() << endl;

    out << "\t\t L2-Cache Miss: " << llcache->getTotalMiss() << " out of " << llcache->getRequest();
    out << " (" << l2missrate << " percent)" << endl;

    out << "\t\t L2-Cache Zero Reuse Evictions: " << llcache->getTotalZeroReuses() << " out of ";
    out << llcache->getEntryRemoved() << " (" << l2zerreouserate << " percent)" << endl;

    out << endl;

    out << "\t\t Requests resulted in " << icache->getRequest() + dcache->getRequest() << " L1 requests, "
        << llcache->getRequest() << " L2 requests, "
        << (*mem).getRequest() << " mem requests " << endl;

    out << endl;

    out << endl;
    out << "------------------------------------------------";
    out << endl;
}

/// Add instruction instrumentation calls
/* ===================================================================== */
void Instruction(INS ins, VOID *v)
{
    // All instructions access the icache
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) AllInstructions,
            IARG_INST_PTR, IARG_END);

    // Reads go to dcache
    if (INS_IsMemoryRead(ins)) {
        INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR) MemoryOp,
                IARG_MEMORYREAD_EA, IARG_INST_PTR,
                IARG_END);

    }

    // Writes go to dcache
    // XXX: note this is not an else branch. It's pretty typical for an x86
    // instruction to be both a read and a write.
    if ( INS_IsMemoryWrite(ins) ) {
        INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE,  (AFUNPTR) MemoryOp,
                IARG_MEMORYWRITE_EA, IARG_INST_PTR,
                IARG_END);
    }
}

/* ===================================================================== */
void Fini(int n, VOID *v)
{
    PrintResults();
}

/* ===================================================================== */
int main(int argc, char *argv[])
{
    if( PIN_Init(argc,argv) ){
        return Usage();
    }

    CreateCaches();

    INS_AddInstrumentFunction(Instruction, 0);

    PIN_AddFiniFunction(Fini, 0);

    PIN_StartProgram();

    return 0;
}
