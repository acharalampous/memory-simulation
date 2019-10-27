/* workingset.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "workingset.h"

/************************/
/*** WORKING SET NODE ***/
/************************/

// Initializes a new WS Node and returns a pointer to it //
WSNode* WSN_Init(char* page){
    WSNode* newNode = malloc(sizeof(WSNode));

    if(newNode == NULL)
        return NULL;

    // Set newNode data //
    strcpy(newNode->page, page);
    newNode->next = NULL;
    newNode->previous = NULL;

    return newNode;
}

void WSN_Print(WSNode* node){
    printf("address: %s\n", node->page);
}


/*******************/
/*** WORKING SET ***/
/*******************/

WorkingSet* WS_Init(int maxSize){
    WorkingSet* newSet = malloc(sizeof(WorkingSet));
    if(newSet == NULL)
        return NULL;

    newSet->start = NULL;
    newSet->end = NULL;
    newSet->totalIns = 0;
    newSet->setSize = 0;
    newSet->maxSize = maxSize;

    return newSet;
}

// Pushes at end of given set, the given node //
int WS_Push(WorkingSet* ws, WSNode* newNode){
    if(ws->setSize == ws->maxSize) // full set
        return -1;

    if(ws->end == NULL){ // empty set
        newNode->next = NULL;
        newNode->previous = NULL;
        ws->start = newNode;
        ws->end = newNode;
    }
    else{ // insert at end and modify previous last node
        newNode->next = NULL;
        ws->end->next = newNode;
        newNode->previous = ws->end;
        ws->end = newNode;
    }
    // Node inserted, increase size
    ws->setSize++;
}

int WS_FindNode(WorkingSet* ws, char* page){
    int position = 0;
    WSNode* temp; // guide through nodes of working set

    temp = ws->start;
    while(temp != NULL){
        char* current = temp->page;
        if(!strcmp(page, current)){ // page found
            return position;
        }
        else{
            position++;
            temp = temp->next;
        }
    }
    return -1; // 404
}

// Delete node in given position of working set//
void WS_Pop(WorkingSet* ws, int position){
    WSNode* temp = ws->start;

    if(temp == NULL) // empty list
        return;

    while(temp != NULL){ // Until end of list
        if(position == 0){ // Until found
            if(ws->start == ws->end){ // list has only one node
                free(temp);
                ws->start = NULL;
                ws->end = NULL;
            }
            else{
                if(temp == ws->start){ // node is first and there is next
                    ws->start = temp->next;
                    temp->next->previous = temp->previous;
                }
                else if(temp == ws->end){
                    ws->end = temp->previous;
                    temp->previous->next = temp->next;
                }
                else{
                    temp->previous->next = temp->next;
                    temp->next->previous = temp->previous;
                }
                free(temp);
            }
            break;
        }
        else{
            temp = temp->next;
            position--;
        }
    }
    ws->setSize--;
}

void WS_Destroy(WorkingSet* ws){
    WSNode* temp = ws->start;
    while(temp != NULL){
        WS_Pop(ws, 0);
        temp = ws->start;
    }
    free(ws);
}

void WS_Print(WorkingSet* ws){
    printf("  totalIns: %d | size: %d | MAXsize: %d\n",ws->totalIns,ws->setSize,ws->maxSize);
    WSNode* node = ws->start;
    int nodeNum = 0;
    while(node != NULL){
        printf("    +Printing node [%d]: ",nodeNum);
        WSN_Print(node);
        node = node->next;
        nodeNum++;
    }
}
