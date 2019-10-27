/* freeframes.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freeframes.h"

//** FRAMES NODE **//

FramesNode* FN_Init(int numFrame){
    FramesNode* newNode = malloc(sizeof(FramesNode));
    if(newNode == NULL)
        return NULL;
    
    newNode->numFrame = numFrame;
    newNode->next = NULL;

    return newNode;
}

void FN_Print(FramesNode* node){
    printf("frame: %d\n",node->numFrame);
}


//** FRAMES LIST **//

// Init new list //
FramesList* FL_Init(int maxFrames){
    FramesList* newList = malloc(sizeof(FramesList));
    if(newList == NULL)
        return NULL;

    newList->start = NULL;
    newList->maxFrames = maxFrames;
    newList->freeFrames = 0;

    int i;
    for(i = maxFrames - 1; i >= 0; i--){
        FL_Push(newList, i);
    }

    return newList;
}

// Push new node at start of list //
void FL_Push(FramesList* list, int i){
    FramesNode* newNode = FN_Init(i);
    newNode->next = list->start;
    list->start = newNode;
    list->freeFrames++;
}

// Delete first node of list and return frame number //
int FL_Pop(FramesList* list){
    FramesNode* temp = list->start;
    int num;

    if(list->freeFrames == 0) // empty list
        return -1;

    num = temp->numFrame; // keep num to be returned

    list->start = temp->next;

    free(temp);
    list->freeFrames--;

    return num;
}

void FL_Destroy(FramesList* list){
    FramesNode* temp = list->start;
    while(temp != NULL){
        FramesNode* current = temp;
        temp = temp->next;
        free(current);
    }
}

void FL_Print(FramesList* list){
    printf("  *[FramesList]:");
    printf(" free: %d | max: %d\n",list->freeFrames,list->maxFrames);
    FramesNode* node = list->start;
    int nodeNum = 0;
    
    while(node != NULL){
        printf("    +Printing node [%d]: ",nodeNum);
        FN_Print(node);
        node = node->next;
        nodeNum++;
    }
}

