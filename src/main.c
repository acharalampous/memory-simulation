/* main.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmu.h"

int main(int argc, char* argv[]){
    if(argc < 6){
        printf("**ERROR** Invalid input. You must give the following: \n");
        printf(" - Replacement Algorithm[LRU or WS]\n");
        printf(" - Number of frames in memory[>0]\n");
        printf(" - Number of references to be executed per step[>0]\n");
        printf(" - Size of working set window[>0 if using WS algorithm]\n");
        printf(" - Number of references in total, to be read from each file[>0]\n");
        return -1;
    }

    char* algorithm = argv[1];
    int FramesNum = atoi(argv[2]); // number of frames in memory
    int q = atoi(argv[3]); // number of sets to read each step
    int wsSize = atoi(argv[4]); // size of working set
    int totalRefs = atoi(argv[5]); // total reference to be read from each file

    if(strcmp(algorithm, "LRU") && strcmp(algorithm, "WS")){
        printf("**ERROR** Invalid input. Choose between LRU and WS algorithm! Abort.\n");
        return -1;
    }

    if(FramesNum <= 0 || q <= 0 || (wsSize <= 0 && (!strcmp(algorithm, "WS"))) || totalRefs <= 0){
        printf("**ERROR** Invalid input. Choose between LRU and WS algorithm! Abort.\n");
        return -1;
    }
    else{
        printf("**Program executed succesfully.**\n");
        printf(" Given Parameters:\n");
        if(!strcmp(algorithm, "WS"))
            printf("  Replacement algorithm: %s with %d size of Working set\n",algorithm, wsSize);
        else
            printf("  Replacement algorithm: %s\n",algorithm);
        printf("  Number Of memory frames: %d\n",FramesNum);
        printf("  Number Of total references to read from each file: %d\n",totalRefs);
        printf("\n--------------------------------------------------------------------\n\n");
    }

    // Initialize MMU //
    myMMU* myMem = MMU_Init(FramesNum, 4, 2, wsSize, algorithm);

    // Open both files //
    FILE* bzipF = fopen("bzip.trace","r");
    FILE* gccF = fopen("gcc.trace","r");
    if(bzipF == NULL || gccF == NULL){
        printf("Error while opening files\n");
        return -1;
    }

    int bzipEOF = 0; // flag if done parsing bzip.trace
    int gccEOF = 0; // flag if done parsing gcc.trace
    int bzipR = 0; // referece read from bzip
    int gccR = 0; // referece read from gcc

    char line[11]; // read logical address from files
    do{
        int i = 0; // number of references read at each step
        for(i = 0; i < q ;i++){
            if(bzipEOF == 1)
                break;

            if(bzipR == totalRefs){ // read all references we wanted
                bzipEOF = 1;
                break;
            }
            if(!(fgets(line, 12, bzipF))){ // reached EOF
                bzipEOF = 1;
                break;
            }
            MMU_newEntry(myMem,line,0);
            bzipR++;
        }

        int j = 0; // number of lines read
        for(j = 0; j < q; j++){
            if(gccEOF == 1)
                break;

            if(gccR == totalRefs){
                gccEOF = 1;
                break;
            }
            if(!(fgets(line, 12, gccF))){
                gccEOF = 1;
                break;
            }
            MMU_newEntry(myMem,line,1);
            gccR++;
        }
    }while((!bzipEOF) || (!gccEOF));

    /* Print all necessary infos */
    printf("**Program execution finished.**\n");
    printf(" Results:\n");
    printf("  Number of Reads from disk: %d\n",myMem->totalReads);
    printf("  Number Of Writes to disk: %d\n",myMem->totalWrites);
    printf("  Number Of total Page Faults: %d\n",myMem->PageFaults);
    printf("  Number Of refs read from bzip.trace: %d\n",bzipR);
    printf("  Number Of refs read from gcc.trace: %d\n",gccR);
    printf("  Number Of refs in total: %d\n", bzipR + gccR);
    printf("  Number Of memory frames: %d\n",FramesNum);

    // 2012 //
    MMU_Destroy(myMem);
    fclose(bzipF);
    fclose(gccF);
}
