PROJECT IMPLEMENTATION STEPS:

1)This project is the design and implementation of a standalone virtual memory manager, where there is a software-managed TLB. The program is responsible to first load a file ontaining a list of logical addresses, next  translate logical addresses into physical addresses for a virtual address space of size 216 = 65,536 bytes, and then  output the value of the byte stored at the translated physical address.
2)This program reads a file containing a list of 32-bit integer numbers, which represent 32-bit logical addresses. It should be noted that the program only deals with 16-bit addresses. Thus, this simulation implements masking for the rightmost 16 bits of each logical address loaded from the file which is given as input to the program.
3)The page table size is 28 bytes; the TLB contains 16 entries. The page size is 2^8^ bytes, which is the same as the frame size. There are a total of 256 frames in the physical memory, meaning that the total physical memory capability is 65,536 bytes (i.e., 256 frames * 256 bytes/frame). The system parameters of the simulated virtual memory is summarized below.
	•	Page table size: 28
	•	Number of TLB entries: 16
	•	Page size: 28 bytes
	•	Frame size: 28 bytes
	•	Number of frames: 256
	•	Physical memory size: 65,536 bytes

4)Here page faults are handled the virtual memory using demand paging.
We use a file called BACKING_store.bin which is a file of 65536 bytes when a page fault occurs it reads a 256 byte page from the file Backing_Store and store the loaded page in physical memory and update the page table as well as TLB thereafter.

Programming  steps:

->1) We used program called VMPagetable.c for writing the code of VM_translation program and VMTypes.c used for the supporting data structures using struct .created logic for page table as well as the tlb-table using structs.

2)Created pagetable and VM table using the accessories with the assumed sizes .To read the file line by line using file commands and extract the Page number and offset using the methods present in VMTypes.c with the help of masking .

3) “translateVirtualAddress” function used for the translation of address checking the pagenumber already present in the TLB tbale and if present assigning the corresponding frame number in tab to the frame_number else we are reading from backstoreusing the function “readFromBackingStore” and the insertion the data into dram table ie, physical memory table.Updating the page table and frame table

4)We are using the “tlbFIFODatainsert” function for inserting the pagenumeber and frame number using the FIFO algorithm by checking the TLBentry and inserting the page number and frame number removing the first entry from the table and replacing with the new entry by shifting the elements of tlb array to the left

5)For given file input virtual address, physical address printed to the console as list corresponding to each virtual address and a translated value stating the value corresponding  in dram physical table .
