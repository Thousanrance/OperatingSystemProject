#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_OF_MEM 65536
#define SIZE_OF_PAGE 256
#define NUM_OF_PAGE 256
#define SIZE_OF_TLB 16
#define NUM_OF_FRAME 128
#define NOT_BE_USED -1

typedef struct TLB_ENTRY
{
	int page_num;
	int frame_num;
	int last_used_time;
} TLB_ENTRY;

typedef struct PAGE_TABLE_ENTRY
{
	int frame_num;
	int valid;		// 1 - valid; 0 - invalid
} PAGE_TABLE_ENTRY;

typedef struct FRAME
{
	char data[SIZE_OF_PAGE]; //size of frame = size of page; 1 char = 1 Byte
	int last_used_time;
} FRAME;


TLB_ENTRY TLB[SIZE_OF_TLB];
PAGE_TABLE_ENTRY page_table[NUM_OF_PAGE];
FRAME physical_memory[NUM_OF_FRAME];

/* for statistics */
int pc;
int page_fault;
int TLB_hit;
int time;


void init()
{
	for (int i = 0; i < SIZE_OF_TLB; i++)
	{
		TLB[i].page_num = NOT_BE_USED;
		TLB[i].frame_num = NOT_BE_USED;
		TLB[i].last_used_time = NOT_BE_USED;
	}

	for (int i = 0; i < NUM_OF_PAGE; i++)
	{
		page_table[i].frame_num= NOT_BE_USED;
		page_table[i].valid = 0;
	}

	for (int i = 0; i < NUM_OF_FRAME; i++)
	{
		physical_memory[i].last_used_time = NOT_BE_USED;
	}

	pc = 0;
	page_fault = 0;
	TLB_hit = 0;
	time = 0;
}

int page_number_in(int addr)
{
	return addr >> 8; //fetch first 8 bits
}

int offset_in(int addr)
{
	return addr & 0x00ff; //fetch last 8 bits
}

/* use LRU strategy */
void TLB_replacement(int page_number, int frame_number)
{
	//printf("TLB replace start\n");
	
	int earliest = time;
	int index = 0;

	for (int i = 0; i < SIZE_OF_TLB; i++)
	{
		if (TLB[i].last_used_time < earliest)
		{
			index = i;
			earliest = TLB[i].last_used_time;
		}
	}

	TLB[index].last_used_time = time;
	TLB[index].frame_num = frame_number;
	TLB[index].page_num = page_number;
	
	//printf("TLB replace over\n");
}

/* use LRU strategy */
int page_replacement(int page_number)
{
	//printf("page replace start\n");
	
	int earliest = time;
	int index = 0;

	/* find the earliest used frame */
	for (int i = 0; i < NUM_OF_FRAME; i++)
	{
		if (physical_memory[i].last_used_time < earliest)
		{
			index = i;
			earliest = physical_memory[i].last_used_time;
		}
	}
	
	/* replace it with the asked page */
	FILE* fp_disk = fopen("BACKING_STORE.bin", "rb");
	fseek(fp_disk, page_number * SIZE_OF_PAGE, SEEK_SET);
	fread(physical_memory[index].data, sizeof(char), SIZE_OF_PAGE, fp_disk);
	fclose(fp_disk);

	/* update the page table */
	for (int i = 0; i < NUM_OF_PAGE; i++)
	{
		if (page_table[i].frame_num == index)
		{
			page_table[i].valid = 0;
		}
	}

	/* update the TLB */
	for (int i = 0; i < SIZE_OF_TLB; i++)
	{
		if (TLB[i].frame_num == index)
		{
			TLB[i].page_num = NOT_BE_USED;
			TLB[i].frame_num = NOT_BE_USED;
			TLB[i].last_used_time = NOT_BE_USED;
		}
	}

	//printf("page replace over\n");
	
	return index;
}

int get_frame_num(int page_number)
{
	//printf("get frame number start\n");

	/* search in TLB */
	for (int i = 0; i < SIZE_OF_TLB; i++)
	{
		if (page_number == TLB[i].page_num)
		{
			TLB_hit++;
			TLB[i].last_used_time = time;
			//printf("get frame number over\n");
			return TLB[i].frame_num;
		}
	}
	/* TLB miss, search in page table */
	if (page_table[page_number].valid == 0) //invalid
	{
		page_fault++;

		page_table[page_number].frame_num = page_replacement(page_number);
		page_table[page_number].valid = 1;
	}
	TLB_replacement(page_number, page_table[page_number].frame_num);
	//printf("get frame number over\n");
	return page_table[page_number].frame_num;
}

int access_memory(int frame_number, int offset)
{
	//printf("access memory start\n");
	physical_memory[frame_number].last_used_time = time;
	//printf("access memory over\n");
	return physical_memory[frame_number].data[offset];
}

int main(int argc, char* argv[])
{
	init();

	FILE* fp_i = fopen(argv[1], "r");
	FILE* fp_o = fopen("output.txt", "w");

	int address;
	int page_number;
	int frame_number;
	int offset;
	int value;

	while (fscanf(fp_i, "%d", &address) != EOF)
	{	
		pc++;

		address &= 0x0000ffff; //fetch last 16 bits
		//printf("%d\n", address);
		page_number = page_number_in(address);
		//printf("%d\n", page_number);
		offset = offset_in(address);
		//printf("%d\n", offset);
		frame_number = get_frame_num(page_number);
		//printf("%d\n", frame_number);
		value = (int)access_memory(frame_number, offset);

		time++;

		fprintf(fp_o, "Virtual address: %d Physical address: %d Value: %d\n", address, (frame_number << 8) + offset, value);
		
		/*
		if(pc == 3)
		{
			return 0;
		}
		*/
	}

	printf("[%d frames]\n", NUM_OF_FRAME);
	printf("Page-fault rate: %lf %%\n", page_fault * 100.0 / pc);
	printf("TLB hit rate: %lf %%\n", TLB_hit * 100.0 / pc);

	fclose(fp_i);
	fclose(fp_o);

	/* check answer */
	FILE* fp_correct = fopen("correct.txt", "r");
	FILE* fp_myAns = fopen("output.txt","r");

	int accept = 1;
	int debugger = 0;
	int correct_value, my_value;

	while (fscanf(fp_correct, "Virtual address: %*d Physical address: %*d Value: %d\n", &correct_value) != EOF)
	{
		fscanf(fp_myAns, "Virtual address: %*d Physical address: %*d Value: %d\n", &my_value);
		debugger++;
		if (my_value != correct_value)
		{
			printf("ERROR on line %d.\n", debugger);
			accept = 0;
		}
	}

	if (accept == 0)
	{
		printf("Wrong Answer\n");
	}
	else
	{
		printf("Accept\n");
	}

	fclose(fp_correct);
	fclose(fp_myAns);

	return 0;
}
