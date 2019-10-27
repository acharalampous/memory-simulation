/* workingset.h */
#pragma once

// Working Set Node //
typedef struct WSNode{
    char page[6]; // Data: Page Number
    struct WSNode* next;
    struct WSNode* previous;
}WSNode;

//  Implementation of Working set, for WS algorithm //
//  Double linked list using FIFO algorithm         //
typedef struct WorkingSet{
    WSNode* start;
    WSNode* end;
    int totalIns; // total insertions made
    int setSize; // size of set
    int maxSize; // Maximum Size of set
}WorkingSet;


/************************/
/*** WORKING SET NODE ***/
/************************/

// Initializes a new WS Node and returns a pointer to it //
WSNode* WSN_Init(char*);

void WSN_Print(WSNode*);


/*******************/
/*** WORKING SET ***/
/*******************/

// Init new working set //
WorkingSet* WS_Init();

// Push new node at end of working set //
int WS_Push(WorkingSet*, WSNode*);

// Find position of node with given address in list //
int WS_FindNode(WorkingSet*, char*);

// Delete node of working set //
void WS_Pop(WorkingSet*, int);

// Destroy given working set //
void WS_Destroy(WorkingSet*);

void WS_Print(WorkingSet*);
