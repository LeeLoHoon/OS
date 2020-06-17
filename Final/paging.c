#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

typedef struct {
	//m-n에 해당
	int bits_for_page_number;
	//n에 해당
	int bits_for_offset;
	//table길이를 입력받음(일반적으로는 2^(m-n)만큼 생김)
	int page_table_length;
	//일반적으로 2^(32-10) 만큼 사용됨
	int *frame_number;

// The following fields should be computed by ReadPageTable() from the above fields.
	//2^n 즉, offset으로 정해짐
	int page_size;
	//이 문제의 경우는 table의 length와 page_number를 비교해 더 작은 것에 맞추어 limit을 잡음
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

	//초기화
	PageTable pt = { 0 };

	int ret = ReadPageTable(argv[1], &pt);
	//읽기 실패
	if(ret == FALSE){
		printf("Failed to read page table from %s\n", argv[1]);
		return -1;
	}

	//현재 주어진 transition을 위한 정보들 출력
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

int ReadPageTable(const char *filename, PageTable *pt)
{
	
	char buffer[30];
	FILE *fp = fopen(filename, "r");
	if(fp == NULL){
		printf("Failed to open file %s\n", filename);
		return FALSE;
	}
	fgets(buffer,sizeof(buffer),fp);

	//bits for page_number
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
	pt->page_size = 1<<pt->bits_for_offset;

	//limit
	
	//error exception when maximum of pages are bigger than page_table_length
	int max_pages = 1<<pt->bits_for_page_number;
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
		//입력값이 table 길이보다 많다면 그만하기
		if(i>=pt->page_table_length) break;
	}
	fclose(fp);

	//테이블 길이 중 입력 받지 못한 나머지 부분 init
	for(i;i<pt->page_table_length;i++){
			pt->frame_number = (int *)realloc(pt->frame_number,(i+1)*sizeof(int));
			pt->frame_number[i] =0;
	}
	return TRUE;
}


void DisplayPageTable(PageTable *pt)
{	
	//maximum page 수는 page bit을 왼쪽으로 1칸 옮긴 바이너리 수이다. ex) 0100(4) --> 1000(2^4)
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
	//만약 bits for offset =2 이라면 p는 오른쪽으로 2칸 값으로 page_num을 의미하게 됨
	int p = logical_addr>>pt->bits_for_offset;
	int off = logical_addr%pt->page_size;

	//bitwise operation : 6>>1 6은 0110이고 오른쪽으로 1칸 이동하는 오퍼레이션 , 6|1 6은 0110이고 1은 0001 이다 이때 | 오퍼레이션은 두 수를 합칠때 둘중 하나라도 1이 있으면 1로 생각한다. 답: 0111

	//여기서 pt->frame_number[p]<<pt->bits_for_offset는 offset bit수만큼 frame_number를 왼쪽으로 이동시키고 offset과 or 오퍼레이션을 통해 전체 physical memory를 만들어 낸다.
	return pt->frame_number[p]<<pt->bits_for_offset|off;
}

