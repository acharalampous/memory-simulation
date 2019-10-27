/* hashtable.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

//** HASH NODE **//
HashNode* HN_Init(char* page, char instr, int frameNum, int pid){
    struct timeval timeSignature; // for timeSignature of addition in PT
    HashNode* newNode = malloc(sizeof(HashNode));

    if(newNode == NULL)
        return NULL;

    // Set newNode data //
    strcpy(newNode->page, page);
    newNode->instr = instr;
    newNode->frameNum = frameNum;
    newNode->pid = pid;
    newNode->next = NULL;
    newNode->previous = NULL;

    // Set time signature //
    gettimeofday(&timeSignature, 0);
    memcpy(&newNode->timeSignature, &timeSignature,sizeof(timeSignature));

    return newNode;
}

void HN_Print(HashNode* node){
    printf("page#: %s | instr: %c | ",node->page,node->instr);
    printf("frame: %d | pid: %d\n",node->frameNum,node->pid);
}


//** HASH LIST **//
HashList* HL_Init(){
    HashList* newList = malloc(sizeof(HashList));
    if(newList == NULL)
        return NULL;

    newList->start = NULL;
    newList->end = NULL;

    return newList;
}

// Pushes at end of given list, the given node //
void HL_Push(HashList* list, HashNode* newNode){
    if(list->end == NULL){ // empty list
        newNode->next = NULL;
        newNode->previous = NULL;
        list->start = newNode;
        list->end = newNode;
    }
    else{
        newNode->next = NULL;
        list->end->next = newNode;
        newNode->previous = list->end;
        list->end = newNode;
    }
}

// Find position of node with given info. If not found return 0 //
int HL_FindNode(HashList* list, char* page, int pid){
    int position = 0;
    HashNode* temp; // guide through nodes of hash list

    temp = list->start;
    while(temp != NULL){ // scan whole list
        char* current = temp->page;
        if((!strcmp(page, current)) && (pid == temp->pid)){ // page found
            return position;
        }
        else{
            position++;
            temp = temp->next;
        }
    }
    return -1; // 404
}

// Returns the first node of given list //
HashNode* HL_GetFirst(HashList* list){
    return list->start;
}

// Delete node in given position of list and return frame //
int HL_Pop(HashList* list, int position, int* toWrite){
    HashNode* temp = list->start;
    int freeFrame = -1; // the frame that will be freed

    if(list->start == NULL) // empty list
        return -1;

    while(temp != NULL){ // Until end of list
        if(position == 0){ // Until found
            freeFrame = temp->frameNum; // keep frame to be freed
            if(list->start == list->end){ // list has only one node
                if(toWrite != NULL && temp->instr == 'W') // write to Disk
                    *toWrite = 1;

                free(temp);
                list->start = NULL;
                list->end = NULL;
            }
            else{
                if(temp == list->start){ // node is first and there is next
                    list->start = temp->next;
                    temp->next->previous = temp->previous;
                }
                else if(temp == list->end){ // node is the last
                    list->end = temp->previous;
                    temp->previous->next = temp->next;
                }
                else{
                    temp->previous->next = temp->next; // node is somewhere in the middle
                    temp->next->previous = temp->previous;
                }
                if(toWrite != NULL && temp->instr == 'W')
                    *toWrite = 1;

                free(temp);
            }
            break;
        }
        else{
            temp = temp->next;
            position--;
        }
    }

    return freeFrame;
}

void HL_Destroy(HashList* list){
    HashNode* temp = list->start;
    while(temp != NULL){ // until all nodes are deleted
        HL_Pop(list, 0, NULL); // delete the first node
        temp = list->start;
    }
}

void HL_Print(HashList* list){
    HashNode* node = list->start;
    int nodeNum = 0;
    while(node != NULL){
        printf("      +Printing block [%d]: ",nodeNum);
        HN_Print(node);
        node = node->next;
        nodeNum++;
    }
}


//** HASH TABLE **//
HashTable* HT_Init(int HashTableSize){
    int i;
    HashTable* newTable = malloc(sizeof(HashTable));
    if(newTable == NULL)
        return NULL;

    newTable->lists = malloc(sizeof(HashList) * HashTableSize);
    newTable->HashTableSize = HashTableSize;

    for(i = 0; i < HashTableSize; i++){
        newTable->lists[i] = HL_Init();
        if(newTable->lists[i] == NULL)
            return NULL;
    }

    return newTable;
}

// Inserts new page in given list's index of hash table //
int HT_newEntry(HashTable* table, HashNode* newNode,int index){
    HL_Push(table->lists[index], newNode);
}

int HT_HashFunction(HashTable* table, char* address){
    char lastbit = address[4];
    int intlast;

    if(lastbit >= 97 && lastbit <= 102) // a - f
        intlast = (int)lastbit - 87;
    else if(lastbit >= 48 && lastbit <= 58) // 0 - 9
        intlast = (int)lastbit - 48;
    else // err
        return -1;

    int index = intlast % table->HashTableSize;

    return index;
}

void HT_Destroy(HashTable* table){
    int i;
    for(i = 0; i < table->HashTableSize; i++){
        HL_Destroy(table->lists[i]);
        free(table->lists[i]);
    }
    free(table->lists);
}

void HT_Print(HashTable* table){
    int i;
    printf("  *[HashTable]\n");
    for(i = 0; i < table->HashTableSize; i++){
        printf("    -Printing HashList [%d]\n",i);
        HL_Print(table->lists[i]);
    }
}
