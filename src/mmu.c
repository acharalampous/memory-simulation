/* MMU.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmu.h"

// Will be used to find least recently page used
float timeElapsed(struct timeval start, struct timeval end)
{
    return (end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f;
}


/***********/
/*** MMU ***/
/***********/

myMMU* MMU_Init(int maxFrames, int HashTableSize,int numProcesses, int wsSize, char* replAlgorithm){
    // Create myMem //
    myMMU* myMem = malloc(sizeof(myMMU));
    if(myMem == NULL)
        return NULL;

    // Create Page Table //
    myMem->PageTable = HT_Init(HashTableSize);
    if(myMem->PageTable == NULL)
        return NULL;

    // Create free frames container
    myMem->freeFrames = FL_Init(maxFrames);

    // Create Working Set
    if(!strcmp(replAlgorithm, "WS")){ // must create working set
        myMem->WorkS = malloc(sizeof(WorkingSet*) * numProcesses);

        int i;
        for(i = 0; i < numProcesses; i++)
            myMem->WorkS[i] = WS_Init(wsSize);
    }
    else
        myMem->WorkS = NULL;

    strcpy(myMem->replAlgorithm, replAlgorithm);
    myMem->numOfProcesses = numProcesses;
    myMem->totalWrites = 0;
    myMem->totalReads = 0;
    myMem->PageFaults = 0;

    return myMem;
}

// Inserts new page in hash table //
void MMU_newEntry(myMMU* myMem, char* logAddress, int pid){
    char page[6];
    char instr;

    // Keep only page number //
    strncpy(page, logAddress, sizeof(page) - 1);

    // Get instruction //
    instr = logAddress[9];

    // LRU OR WS //
    if(!strcmp(myMem->replAlgorithm, "LRU")){ // Using LRU
        MMU_LRUInsertion(myMem, page, instr, pid);
    }
    else if(!strcmp(myMem->replAlgorithm, "WS"))
        MMU_WSInsertion(myMem, page,instr,pid);

    if(instr == 'R')
        myMem->totalReads++;
}

void MMU_LRUInsertion(myMMU* myMem, char* page, char instr, int pid){
    int index; // index of list in HashTable
    int position; // place in list
    HashNode* newNode;

    // Find correct place for address in hash table //
    index = HT_HashFunction(myMem->PageTable, page);

    // Point to list we are about to insert address //
    HashList* list = myMem->PageTable->lists[index];

    //  check if the address exists in given index  //
    //  and return its position, if it exists       //
    position = HL_FindNode(list, page, pid);

    if(position != -1){ // address exists in "memory"
        int frame = HL_Pop(list, position, NULL); // (remove to) update timesignature
        newNode = HN_Init(page, instr, frame, pid); // create the updated page
        HL_Push(list, newNode); // push it to end
    }
    else{ // doesnt exist in memory
        myMem->PageFaults++;

        if(myMem->freeFrames->freeFrames == 0){ // no frames left
            int frame = MMU_VictimizeLRU(myMem); // free frame
            newNode = HN_Init(page, instr, frame, pid);
            HL_Push(list, newNode);
        }
        else{
            int frame = FL_Pop(myMem->freeFrames); // get free frame
            newNode = HN_Init(page, instr, frame, pid);
            HL_Push(list, newNode);
        }
    }
}

void MMU_WSInsertion(myMMU* myMem, char* page, char instr, int pid){
    WorkingSet* ws = myMem->WorkS[pid];
    int index; // index of list in HashTable
    int position; // place in list
    HashNode* newNode;

    // UPDATE WORKING SET

    if(ws->totalIns >= ws->maxSize){ // maxSize inserts were made in ws
        if(ws->setSize == ws->maxSize){ // full working set
            int pos = WS_FindNode(ws, page); // search for page in working set
            if(pos == -1){ // page not in working set
                myMem->PageFaults++;
                WS_Pop(ws, 0); // Delete first node
                WS_Push(ws, WSN_Init(page));
                ws->totalIns++;
            }
            else if(pos == 0){// page is in working set and first position
                WS_Pop(ws,pos);
                WS_Push(ws, WSN_Init(page));
            }
            else{ // page in working set and not first position
                WS_Pop(ws,pos); // Update page
                WS_Pop(ws, 0); // we need to delete first page
                WS_Push(ws, WSN_Init(page));
            }
        }
        else{ // not full ws
            int pos = WS_FindNode(ws, page); // search for page in working set
            if(pos != -1){ // not in set
                myMem->PageFaults++;
                WS_Push(ws, WSN_Init(page));
                ws->totalIns++;
            }
            else{ // already in set
                WS_Pop(ws,pos); // Update page
                WS_Push(ws, WSN_Init(page));
            }
        }
    }
    else{ // did not make maxSize insertions
        int pos = WS_FindNode(ws, page);
        if(pos == -1){ // page not in working set
            WS_Push(ws, WSN_Init(page));
            ws->totalIns++;
        }
        else{ // page already in working set
            WS_Pop(ws, pos); // Update page
            WS_Push(ws, WSN_Init(page));
        }
    }

    // UPDATE PAGE TABLE

    // Find correct place for page in hash table //
    index = HT_HashFunction(myMem->PageTable, page);

    // Point to list we are about to insert page //
    HashList* list = myMem->PageTable->lists[index];

    //  check if the page exists in given index  //
    //  and return its position, if it exists    //
    position = HL_FindNode(list, page, pid);
    if(position == -1){ // page doesnt exist in "memory"

        if(myMem->freeFrames->freeFrames == 0){ // no frames left
            MMU_VictimizeWS(myMem, pid);
        }

        int frame = FL_Pop(myMem->freeFrames); // get free frame
        newNode = HN_Init(page, instr, frame, pid);
        HL_Push(list, newNode);
    }
}


int MMU_VictimizeLRU(myMMU* myMem){
    int i;
    float timeDifference;
    float mostDifference = 0.0;
    int victim = -1;
    struct timeval currentTime, addTime; // for timeSignal
    gettimeofday(&currentTime, 0); // set timeSignal

    HashTable* ht = myMem->PageTable;

    for(i = 0; i < ht->HashTableSize; i++){
        HashNode* firstNode = HL_GetFirst(ht->lists[i]); // point to first node, its the oldest of list
        if(firstNode != NULL){
            memcpy(&addTime, &firstNode->timeSignature, sizeof(addTime));
            timeDifference = timeElapsed(addTime, currentTime);
            if(timeDifference > mostDifference){
                victim = i;
                mostDifference = timeDifference;
            }
        }
    }

    int toWrite = 0;
    int frame = HL_Pop(ht->lists[victim], 0, &toWrite);
    if(toWrite == 1)
        myMem->totalWrites++;

    return frame;
}

void MMU_VictimizeWS(myMMU* myMem, int pid){
    HashTable* pTable = myMem->PageTable;
    WorkingSet* ws = myMem->WorkS[pid];
    int i;

    // remove all pages from all lists that are not in ws of process//
    for(i = 0; i < pTable->HashTableSize; i++){
        int pos = 0;
        HashNode* temp = HL_GetFirst(pTable->lists[i]);
        while(temp != NULL){
            if(temp->pid == pid && (WS_FindNode(ws, temp->page) == -1)){ // page not in ws

                int toWrite = 0;
                int freeFrame = HL_Pop(pTable->lists[i], pos, &toWrite);
                if(toWrite == 1)
                    myMem->totalWrites++;

                FL_Push(myMem->freeFrames, freeFrame);
            }
            temp = temp->next;
            pos++;
        }
    }
}


void MMU_Destroy(myMMU* myMem){
    HT_Destroy(myMem->PageTable);
    FL_Destroy(myMem->freeFrames);
    if(!strcmp(myMem->replAlgorithm, "WS")){ // must destroy working set
        int i;
        for(i = 0; i < myMem->numOfProcesses; i++)
            WS_Destroy(myMem->WorkS[i]);
        free(myMem->WorkS);
    }
    free(myMem->PageTable);
    free(myMem->freeFrames);
    free(myMem);
}

void MMU_Print(myMMU* myMem){
    printf("..........................................................\n");
    printf("[MMU]:");
    printf(" procs: %d | writes: %d | reads: %d | ",myMem->numOfProcesses,myMem->totalWrites,myMem->totalReads);
    printf("Page Faults: %d | alg: %s\n",myMem->PageFaults,myMem->replAlgorithm);
    HT_Print(myMem->PageTable);
    FL_Print(myMem->freeFrames);
    if(!strcmp(myMem->replAlgorithm, "WS")){
        int i;
        for(i = 0; i < myMem->numOfProcesses; i++){
            printf("  *[Working Set[%d]]: ",i);
            WS_Print(myMem->WorkS[i]);
        }
    }
    printf("..........................................................\n");
}
