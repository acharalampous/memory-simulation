/* freeframes.h */
#pragma once

// A Node that holds a number of free frame //
typedef struct FramesNode{
    int numFrame;
    struct FramesNode* next;
}FramesNode;

//  Container of all available(free) frames //
//  List that uses LIFO algorithm           //
typedef struct FramesList{
    FramesNode* start;
    int freeFrames; // number of nodes
    int maxFrames; // number of maximum nodes
}FramesList;


/*******************/
/*** FRAMES NODE ***/
/*******************/

// Initializes a new Frames Node and returns a pointer to it //
FramesNode* FN_Init(int);

void FN_Print(FramesNode*);


/*******************/
/*** FRAMES LIST ***/
/*******************/

// Init new list //
FramesList* FL_Init();

// Push new node at start of list //
void FL_Push(FramesList*, int);

// Delete first node of list //
int FL_Pop(FramesList*);

void FL_Destroy(FramesList*);

void FL_Print(FramesList*);

