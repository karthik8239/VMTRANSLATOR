
#define VMTYPES_H_

typedef struct vmTable_t {
    int *pageNumArr; // page number array
    int *frameNumArr; // frame number array for this
    int length;
    int pageFaultCount;
    int tlbHitCount;
    int tlbMissCount;
} vmTable_t;

vmTable_t* createVMtable(int length);//CREATES VM TABLE

void displayTable(vmTable_t** tableToView);

void freeVMtable(vmTable_t** table);

void freeDRAM(int*** dblPtrArr, int frameCount);

int getPageNumber(int mask, int value, int shift);

int getOffset(int mask, int value);

int** dramAllocate(int frameCount, int blockSize);
