#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

typedef struct {
	int bits_for_page_number;
	int bits_for_offset;
	int page_table_length;
	int *frame_number;

// The following fields should be computed by ReadPageTable() from the above fields.
	int page_size;
	int limit; 
} PageTable;


int PowNum(int a, int b);
int ReadPageTable(const char *filename, PageTable *pt);
void DisplayPageTable(PageTable *pt);
void DestroyPageTable(PageTable *pt);
int Translate(int logical_addr, PageTable *pt);

int main(int argc, char *argv[])
{
	if(argc < 2){
		printf("Usage: %s <pagetable_file>\n", argv[0]);
		return 0;
	}

	PageTable pt = { 0 };
	int ret = ReadPageTable(argv[1], &pt);
	if(ret == FALSE){
		printf("Failed to read page table from %s\n", argv[1]);
		return -1;
	}

	DisplayPageTable(&pt);

	int addr_limit = pt.limit;

	int step = 1;
	if(addr_limit > 256)
		step = addr_limit / 256;

	for(int logical_addr = 0; logical_addr < addr_limit; logical_addr += step){
		int physical_addr = Translate(logical_addr, &pt);
		printf("%04d (0x%04x) ==> %04d (0x%04x)\n", logical_addr, logical_addr, physical_addr, physical_addr);
	}

	DestroyPageTable(&pt);

	return 0;
}

int PowNum(int a, int b){
	if(b==0) return 1;
	else if(b==1)	return a;
	else return a*PowNum(a,b-1);
}

int ReadPageTable(const char *filename, PageTable *pt)
{
	
	char buffer[30];
	FILE *fp = fopen(filename, "r");
	if(fp == NULL){
		printf("Failed to open file %s\n", filename);
		return FALSE;
	}
	fgets(buffer,sizeof(buffer),fp);

	//page_number
	char *ptr = strtok(buffer," ");
	ptr = strtok(NULL," ");
	pt->bits_for_page_number = atoi(ptr);
	
	//bits_for_offset 
	fgets(buffer,sizeof(buffer),fp);
	ptr = strtok(buffer," ");
	ptr = strtok(NULL," ");
	pt->bits_for_offset = atoi(ptr);

	//page_table_length 
	fgets(buffer,sizeof(buffer),fp);
	ptr = strtok(buffer," ");
	ptr = strtok(NULL," ");
	pt->page_table_length = atoi(ptr);

	//page_size
	pt->page_size = PowNum(2,pt->bits_for_offset);

	//limit
	
	//error exception when maximum of pages are bigger than page_table_length
	int max_pages = PowNum(2,pt->bits_for_page_number);
	if(pt->page_size*pt->page_table_length<=max_pages*pt->page_size)
		pt->limit = pt->page_table_length*pt->page_size;
	else pt->limit = max_pages*pt->page_size;

	//frame_number

	//first number
	fgets(buffer,sizeof(buffer),fp);
	ptr=strtok(buffer," ");
	pt->frame_number = (int *)malloc(sizeof(int));
	pt->frame_number[0] = atoi(ptr);

	//remain number
	int i=1;
	while(!feof(fp)){
		fgets(buffer,sizeof(buffer),fp);
		ptr=strtok(buffer," ");
		pt->frame_number = (int *)realloc(pt->frame_number,(i+1)*sizeof(int));
		pt->frame_number[i] = atoi(ptr);
		i++;
		if(i>=pt->page_table_length) break;
	}
	fclose(fp);
	return TRUE;
}


void DisplayPageTable(PageTable *pt)
{
	printf("BITS_FOR_PAGE_NUMBER %d (maximum # of pages = %d)\n",
		pt->bits_for_page_number, 1 << pt->bits_for_page_number);
	printf("BITS_FOR_OFFSET %d (page_size = %d)\n",
		pt->bits_for_offset, pt->page_size);
	printf("PAGE_TALBLE_LENGTH %d (address limit = %d)\n", pt->page_table_length, pt->limit);

	for(int i = 0; i < pt->page_table_length; i++)
		printf("%3d: %d\n", i, pt->frame_number[i]);
}

void DestroyPageTable(PageTable *pt)
{
	free(pt->frame_number);
}

int Translate(int logical_addr, PageTable *pt)
{
	int p = logical_addr>>pt->bits_for_offset;
	int off = logical_addr%pt->page_size;

	return pt->frame_number[p]<<pt->bits_for_offset|off;
}

