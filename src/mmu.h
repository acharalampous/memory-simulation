/* mmu.h */
#pragma once
#include "hashtable.h"
#include "freeframes.h"
#include "workingset.h"

// Implementation of MMU //
typedef struct myMMU{
    HashTable* PageTable;
    FramesList* freeFrames;
    WorkingSet** WorkS; // will hold number of Working Sets
    int numOfProcesses; // number of Processes using MMU
    int totalWrites; // number of total writes
    int totalReads; // number of total read
    int PageFaults; // number of total page faults
    char replAlgorithm[5]; // replacement algorithm: LRU or WS
}myMMU;

//  Initializes myMMU level and returns pointer to it //
myMMU* MMU_Init(int, int, int, int, char*);

// New address in MMU
void MMU_newEntry(myMMU*, char*, int);

// Insert new entry using LRU //
void MMU_LRUInsertion(myMMU*, char*, char, int);

// Insert new entry using WS //
void MMU_WSInsertion(myMMU*, char*, char, int);

// Make space in pagetable using LRU //
int MMU_VictimizeLRU(myMMU*);

// Make space in pagetable using WS //
void MMU_VictimizeWS(myMMU*, int);

// Destroy MMU //
void MMU_Destroy(myMMU*);

void MMU_Print(myMMU*);

