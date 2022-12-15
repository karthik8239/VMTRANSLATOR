#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "VMTypes.h" 

vmTable_t* createVMtable(int length)
{
    vmTable_t* new_table = malloc(sizeof(vmTable_t));
    new_table->length = length;
    new_table->pageNumArr = malloc(sizeof(int) * length);
    new_table->frameNumArr = malloc(sizeof(int) * length);
    new_table->pageFaultCount = 0;
    new_table->tlbHitCount = 0;
    new_table->tlbMissCount = 0;

    for (int i = 0; i < length; i++) {
        new_table->pageNumArr[i] = 0;
    }

    // If there is not enough memory on the heap to make a call to malloc() // Notify and Exit
    if(new_table == NULL || new_table->pageNumArr == NULL || new_table->frameNumArr == NULL) {
        printf("Error - Could not allocate a new Virtual Memory Addressing Table!\r\n");
        exit(-1);
    }
    return new_table;
}

void freeVMtable(vmTable_t** table) {
    if ((*table)->pageNumArr != NULL) {
        free((*table)->pageNumArr);
    }
    if ((*table)->frameNumArr != NULL) {
        free((*table)->frameNumArr);
    }
    free(*table);
}

void displayTable(vmTable_t** tableToView)
{
    printf("the table display start as follows");
    for(int i=0;i<(*tableToView)->length;i++){
      printf("Index(%d) page num: %d\tFrame Number: %d\n", i, (*tableToView)->pageNumArr[i], (*tableToView)->frameNumArr[i]);
    }
    printf("table display ends");
}

int** dramAllocate(int frameCount, int blockSize)
{
    int** temp;
    temp = malloc(frameCount * sizeof(int*));
    for(int i = 0; i < frameCount; i++) {
        temp[i] = (int*)malloc(sizeof(int) * blockSize);
        for(int j = 0; j < blockSize; j++) {
            temp[i][j] = 0;
        }
    }
    if(temp == NULL) {
        printf("Error - Could not allocate memory\r\n");
        exit(-1);
    }
    return temp;
}

//freeing the allocated RAM table
void freeDRAM(int*** dblPtrArr, int frameCount)
{
  for (int i = 0; i < frameCount; i++) {
      if ((*dblPtrArr)[i] != NULL) {
          free((*dblPtrArr)[i]);
      }
  }
  free(*dblPtrArr);
}

  // mask ->   The int masking value we will use to perform AND operation w.r.t. value
   // value -> The int value we wish to mask
  // shift ->  The relative number of bits we want to shift right after the bitwise operation
//getting the pagenumber using mask and shift
int getPageNumber(int mask, int value, int shift){
return ((value&mask)>>shift);
}

//getting the offset value
int getOffset(int mask, int value){
   return value & mask;
}

