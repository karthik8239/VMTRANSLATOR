//command line arguments "vm <file>” 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>
#include <time.h>
#include "VMTypes.h"

#define FRAME_SIZE        256       // Size of each frame
#define TOTAL_FRAME_COUNT 256       //  number of frames in physical memory
#define PAGE_MASK         0xFF00    // Masks everything but the page number
#define OFFSET_MASK       0xFF      // Masks everything but the offset
#define SHIFT             8         // Amount to shift when doing bit mask
#define TLB_SIZE          16        // size of the TLB
#define PAGE_TABLE_SIZE   256       // size of the page table
#define MAX_ADDR_LEN      10        // The number of characters to read for each line from input file.
#define PAGE_READ_SIZE    256       // Number of bytes to read


vmTable_t* tlbTable; 
vmTable_t* pageTable;
int** dram; 


int nextTLBentry = 0;
int nextPage = 0;
int nextFrame = 0;

FILE* address_file;
FILE* backing_store;

char addressReadBuffer[MAX_ADDR_LEN];
int virtual_addr;
int page_number;
int offset_number;

signed char fileReadBuffer[PAGE_READ_SIZE];

signed char translatedValue;

void translateVirtualAddress();
void readFromBackingStore(int pageNumber);
void tlbFIFODatainsert(int pageNumber, int frameNumber);

int main(int argc, char *argv[])
{
	//	file descriptors
	if(argc!=2){
		printf("less arguments needed error");
	}

	address_file= fopen(argv[1],"r");//opening the file

     if(argv[1]==NULL){
		printf("Error missing the file name");
		exit(1);
	 }
	 tlbTable = createVMtable(TLB_SIZE);//creating the translation look aside buffer table
	 pageTable = createVMtable(PAGE_TABLE_SIZE);//creatng the page table with size
	 dram = dramAllocate(TOTAL_FRAME_COUNT, FRAME_SIZE);
	 int translationCount = 0;

    backing_store = fopen("BACKING_STORE.bin", "rb");
	 printf("\nNumber of logical pages: %d\nPage size: %d bytes\nPage Table Size: %d\nTLB Size: %d entries\nNumber of Physical Frames: %d\nPhysical Memory Size: %d bytes", PAGE_TABLE_SIZE, PAGE_READ_SIZE, PAGE_TABLE_SIZE, TLB_SIZE, FRAME_SIZE, PAGE_READ_SIZE * FRAME_SIZE);
    
	//reading the file line by line
	while (fgets(addressReadBuffer, MAX_ADDR_LEN, address_file) != NULL) {
		   virtual_addr = atoi(addressReadBuffer); //assic to int conversion;
		   page_number = getPageNumber(PAGE_MASK, virtual_addr, SHIFT);//get page number from utility VMTypes.h
		   offset_number = getOffset(OFFSET_MASK ,virtual_addr);
		   translateVirtualAddress();
		   translationCount++;
	}
	 fclose(address_file);
    fclose(backing_store);
    freeVMtable(&tlbTable);
    freeVMtable(&pageTable);
    freeDRAM(&dram, TOTAL_FRAME_COUNT);
	return 0;	
}

//function to translate address.
void translateVirtualAddress(){
	 int frame_number = -1;

	 for(int i =0;i<tlbTable->length;i++){
		if(tlbTable->pageNumArr[i]== page_number){
			frame_number = tlbTable->frameNumArr[i];
			tlbTable->tlbHitCount++;
			break;
		}
	 }

	 if (frame_number == -1) {
        tlbTable->tlbMissCount++; 
        for(int i = 0; i < nextPage; i++){
            if(pageTable->pageNumArr[i] == page_number){  
                frame_number = pageTable->frameNumArr[i];
                break; 
            }
        }
  //If there is no frame present in tlb get the page number from backing store
	 if (frame_number == -1) {
         pageTable->pageFaultCount++;
		 readFromBackingStore(page_number);
	     frame_number = nextFrame - 1; 
		}
      }
	  tlbFIFODatainsert(page_number, frame_number);
	  translatedValue = dram[frame_number][offset_number];
	 printf("\nVirtual address: %d\t\tPhysical address: %d\t\tValue: %d", virtual_addr, (frame_number << SHIFT) | offset_number, translatedValue);

}

//when reading for the first time  and bringing back physical memory in the table
void readFromBackingStore(int pageNumber){
  if (fseek(backing_store,pageNumber * PAGE_READ_SIZE,SEEK_SET) !=0 ){
	   fprintf(stderr, "Error seeking in backing store\n");
  }
  if (fread(fileReadBuffer, sizeof(signed char), PAGE_READ_SIZE, backing_store) == 0) {
        fprintf(stderr, "Error reading from backing store\n");
    }
	//Load the bits into first avaialble memory in the 2D array
	for(int i=0;i<PAGE_READ_SIZE;i++){
		dram[nextFrame][i] =fileReadBuffer[i];
	}
	pageTable->pageNumArr[nextPage] = pageNumber;
	pageTable->frameNumArr[nextPage] = nextFrame;
	nextFrame++;
	nextPage++;
}

//function to insert page number and frame number into TLB with a FIFO replacement
void tlbFIFODatainsert(int pageNumber, int frameNumber){
  int i;
  //If it's already in the TLB,break
  for(int i=0;i<nextTLBentry;i++){
	if(tlbTable->pageNumArr[i]==pageNumber){
		break;
	}
  }
  if(i== nextTLBentry){
	 if(nextTLBentry<TLB_SIZE){
        tlbTable->pageNumArr[nextTLBentry] = pageNumber;
		tlbTable->frameNumArr[nextTLBentry] = frameNumber;
	 } 
	 //replace the last tlb entry with our new entry
	 else {
	    tlbTable->pageNumArr[nextTLBentry-1]=pageNumber;
		tlbTable->frameNumArr[nextTLBentry-1]=frameNumber;	
	 

	 for( i=0;i<TLB_SIZE-1;i++){
		tlbTable->pageNumArr[i] =tlbTable->pageNumArr[i+1];
		tlbTable->frameNumArr[i] = tlbTable->frameNumArr[i+1];
	   }
	 }
  }
  //if index not equal to number of entries
    else {
		for(i=1;i<nextTLBentry-1;i++){
			tlbTable->pageNumArr[i] = tlbTable->pageNumArr[i+1];
			tlbTable->frameNumArr[i] =tlbTable->frameNumArr[i+1];
		}
		//if there is space in the array keep the page number and frame number on end
		if(nextTLBentry < TLB_SIZE){
           tlbTable->pageNumArr[nextTLBentry] =pageNumber;
		   tlbTable->frameNumArr[nextTLBentry] = frameNumber;
		}
		//if no space keep page number and frame number at end-1
		else{ 
			tlbTable->pageNumArr[nextTLBentry-1] = pageNumber;
			tlbTable->frameNumArr[nextTLBentry-1] = frameNumber;
		}
	}
	if(nextTLBentry < TLB_SIZE){
		nextTLBentry++;
	}
  }


