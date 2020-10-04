#include "pin.H"

#include <iostream>
#include <fstream>
#include <cassert>

#include "globals.h"
#include "tlb.h"
#define STORE_ALLOCATION STORE_ALLOCATE
#include "cache.h"



/* ===================================================================== */
/* Commandline Switches                                                  */
/* ===================================================================== */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,    "pintool",
    "o", "cslab_cache.out", "specify dcache file name");

// Tlb
KNOB<UINT32> KnobTlbSizeEntries(KNOB_MODE_WRITEONCE, "pintool",
    "TLBe","64", "TLB size in #entries");
KNOB<UINT32> KnobPageSize(KNOB_MODE_WRITEONCE, "pintool",
    "TLBp","4096", "Page size in bytes");
KNOB<UINT32> KnobTlbAssociativity(KNOB_MODE_WRITEONCE, "pintool",
    "TLBa","4", "TLB associativity (1 for direct mapped)");

// L1Cache
KNOB<UINT32> KnobL1CacheSize(KNOB_MODE_WRITEONCE, "pintool",
    "L1c","32", "L1 cache size in kilobytes");
KNOB<UINT32> KnobL1BlockSize(KNOB_MODE_WRITEONCE, "pintool",
    "L1b","64", "L1 cache block size in bytes");
KNOB<UINT32> KnobL1Associativity(KNOB_MODE_WRITEONCE, "pintool",
    "L1a","8", "L1 cache associativity (1 for direct mapped)");

// L2Cache
KNOB<UINT32> KnobL2CacheSize(KNOB_MODE_WRITEONCE, "pintool",
    "L2c","256", "L2 cache size in kilobytes");
KNOB<UINT32> KnobL2BlockSize(KNOB_MODE_WRITEONCE, "pintool",
    "L2b","64", "L2 cache block size in bytes");
KNOB<UINT32> KnobL2Associativity(KNOB_MODE_WRITEONCE, "pintool",
    "L2a","8", "L2 cache associativity (1 for direct mapped)");

// Prefetcher
KNOB<UINT32> KnobL2PrefetchLines(KNOB_MODE_WRITEONCE, "pintool",
    "L2prf","0", "Number of lines to prefetch to L2 (0 disables prefetching)");

/* ===================================================================== */

/* ===================================================================== */
/* Global Variables                                                      */
/* ===================================================================== */
typedef SINGLE_LEVEL_TLB<TLB_SET::LRU> TLB_T;
TLB_T *tlb;

typedef TWO_LEVEL_CACHE<CACHE_SET::LRU> CACHE_T;
CACHE_T *two_level_cache;

UINT64 total_cycles, total_instructions;
std::ofstream outFile;

/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool represents a 2-level tlb & cache simulator.\n\n";
    cerr << KNOB_BASE::StringKnobSummary();
    cerr << endl;
    return -1;
}

/* ===================================================================== */

VOID Load(ADDRINT addr)
{
    // get the address translation from Virtual to Physical address space
    // note: only for timing simulation purpose
    // "addr" is virtual and remains unchanged for accessing the cache hierarchy
    total_cycles += tlb->Access(addr, TLB_T::ACCESS_TYPE_LOAD);
    // load the data from the cache hierarchy
    total_cycles += two_level_cache->Access(addr, CACHE_T::ACCESS_TYPE_LOAD);
}

VOID Store(ADDRINT addr)
{
    // get the address translation from Virtual to Physical address space
    // note: only for timing simulation purpose
    // "addr" is virtual and remains unchanged for accessing the cache hierarchy
    total_cycles += tlb->Access(addr, TLB_T::ACCESS_TYPE_STORE);
    // store the data to the cache hierarchy
    total_cycles += two_level_cache->Access(addr, CACHE_T::ACCESS_TYPE_STORE);
}

VOID count_instruction()
{
    total_instructions++;
    total_cycles++;

    //keep stats per 10M instructions
    if (total_instructions % 10000000==0){
        outFile << "INSTRUCTIONS: " << total_instructions / 10000000 << " *x10M IPC: " << (double)total_instructions / (double)total_cycles << "\n";
    }
}

VOID Instruction(INS ins, void * v)
{
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Instrument each memory operand. If the operand is both read and written
    // it will be processed twice.
    // Iterating over memory operands ensures that instructions on IA-32 with
    // two read operands (such as SCAS and CMPS) are correctly handled.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++) {
        if (INS_MemoryOperandIsRead(ins, memOp)) {
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) Load,
                                     IARG_MEMORYOP_EA, memOp, IARG_END);
        }
        if (INS_MemoryOperandIsWritten(ins, memOp)) {
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) Store,
                                     IARG_MEMORYOP_EA, memOp, IARG_END);
        }
    }

    // Count each and every instruction
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_instruction, IARG_END);
}

/* ===================================================================== */

VOID Fini(int code, VOID * v)
{
    // Report total instructions and total cycles
    outFile << "--------\n";
    outFile << "Total Statistics\n";
    outFile << "--------\n";
    outFile << "Total Instructions: " << total_instructions << "\n";
    outFile << "Total Cycles: " << total_cycles << "\n";
    outFile << "IPC: " << (double)total_instructions / (double)total_cycles << "\n";
    outFile << "\n";

    // Report Cache configuration + statistics
    outFile << tlb->PrintDetails("");
    outFile << tlb->StatsLong("");
    outFile << "\n\n";
    outFile << two_level_cache->PrintCache("");
    outFile << two_level_cache->StatsLong("");

    outFile.close();
}

VOID roi_begin()
{
    INS_AddInstrumentFunction(Instruction, 0);
}

VOID roi_end()
{
    // We need to manually call Fini here because it is not called by PIN
    // if PIN_Detach() is encountered.
    Fini(0, 0);
    PIN_Detach();
}

VOID Routine(RTN rtn, void *v)
{
    RTN_Open(rtn);

    if (RTN_Name(rtn) == "__parsec_roi_begin")
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)roi_begin, IARG_END);
    if (RTN_Name(rtn) == "__parsec_roi_end")
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)roi_end, IARG_END);

    RTN_Close(rtn);
}

/* ===================================================================== */

int main(int argc, char *argv[])
{
    PIN_InitSymbols();

    if(PIN_Init(argc,argv))
        return Usage();

    // Open output file
    outFile.open(KnobOutputFile.Value().c_str());

    // Initialize single level Tlb
    tlb = new TLB_T("Single level Tlb hierarchy",
                                  KnobTlbSizeEntries.Value(),
                                  KnobPageSize.Value(),
                                  KnobTlbAssociativity.Value());
    // Initialize two level Cache
    two_level_cache = new CACHE_T("Two level Cache hierarchy",
                                  KnobL1CacheSize.Value() * KILO,
                                  KnobL1BlockSize.Value(),
                                  KnobL1Associativity.Value(),
                                  KnobL2CacheSize.Value() * KILO,
                                  KnobL2BlockSize.Value(),
                                  KnobL2Associativity.Value(),
                                  KnobL2PrefetchLines.Value());

    // Instrument function calls in order to catch __parsec_roi_{begin,end}
    RTN_AddInstrumentFunction(Routine, 0);

    // Called when the instrumented application finishes its execution
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
