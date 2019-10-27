/* hashtable.h */
#pragma once
#include <sys/time.h>

// Node will hold info about each page in hash table //
typedef struct HashNode{
    char page[6]; // Data: Page Number of Logical Address
    char instr; // Instruction: Read or Write
    int frameNum; // Number of frame in memory
    struct timeval timeSignature; // Time that it was added(or modified) in PT
    int pid; // Process that did the instruction
    struct HashNode* next;
    struct HashNode* previous;
}HashNode;

// A double linked FIFO list //
typedef struct HashList{
    HashNode* start; // points at start of list
    HashNode* end; // points at end of list
}HashList;

// Hash table that contains number of HashLists //
typedef struct HashTable{
    HashList** lists; // lists of hash table
    int HashTableSize; // number of lists
}HashTable;


/*****************/
/*** HASH NODE ***/
/*****************/

// Initializes a new Hash Node and returns a pointer to it  //
// Takes as arguments the page number, R or W instruction,  //
// number of frame to be occupied and process id            //
HashNode* HN_Init(char*, char, int, int);

// Prints all info about given hashnode //
void HN_Print(HashNode*);


/*****************/
/*** HASH LIST ***/
/*****************/

// Init new Hash list //
HashList* HL_Init();

// Push new node at end of list //
void HL_Push(HashList*, HashNode*);

// Find position of node with given address in list //
int HL_FindNode(HashList*, char*, int);

// Returns the first node of given list //
HashNode* HL_GetFirst(HashList*);

// Delete node in given position of list    //
// Returns its frameNum to be added to      //
// Free frames struct and check if W instr  //
int HL_Pop(HashList*, int, int*);

// Destroy given list //
void HL_Destroy(HashList*);

void HL_Print(HashList*);


/******************/
/*** HASH TABLE ***/
/******************/

// Init Hash Table //
HashTable* HT_Init(int);

// Inserts new page in hash table //
int HT_newEntry(HashTable*, HashNode*,int index);

//  Given the address as string, it finds the index //
//  that's it stored in HashTable using mod %       //
int HT_HashFunction(HashTable*, char*);

// Fully Destroys given table
void HT_Destroy(HashTable*);

void HT_Print(HashTable*);
