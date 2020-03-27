#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int ReadTextLine(int fd, char str[], int max_len);

#define BUFFER_SIZE 512
char buffer[BUFFER_SIZE];
int buffer_size = 0;
int buffer_pos = 0;
//char master_string[100];
char master_str[100];
char temp[100];
int core;
char model[100];
int memory;
float  loadavg1, loadavg5, loadavg15;

int main()
{
    int Mfd = open("/proc/cpuinfo",O_RDONLY);
    char Mstr[100];
    //ReadTextLine(Mfd,Mstr,10);
    while(1){
		char dump1[20];
		char dump2[20];

		int t = ReadTextLine(Mfd,Mstr,100);
        char *ptr = strtok(temp, ":");
		printf("%s\n",ptr);
		printf("%d\n",(int)strlen(ptr));
        
        if(!strncmp("cpu cores",ptr,9)){
			sscanf(master_str,"%*s %*s %d",&core);
			printf("%d\n",core);
			break;
		}
		if(!strncmp("model name",ptr,10)){
			ptr = strtok(NULL,"\n");
			strcpy(model,ptr+1);
			printf("!!!!!!!!!%s\n",model);
	    	//break;
		}
		//printf("%s\n",ptr);

    }
	printf("out\n");
	close(Mfd);
	return 0;
}

int ReadTextLine(int fd, char str[], int max_len)
{

    int i = 0;
    int j = 0;
    int ret = 0;
    // if current position is 0, reset buffer size and pos
    if (lseek(fd, 0, SEEK_CUR) == 0)
        buffer_pos = buffer_size = 0;
    while (j < max_len - 1)
    {
        if (buffer_pos == buffer_size)
        {
            buffer[0] = 0;
            buffer_size = read(fd, buffer, BUFFER_SIZE);
            buffer_pos = 0;
        }
        if (buffer_size == 0)
        {
            if (j == 0)
                ret = EOF;
            break;
        }
        while (j < max_len - 2 && buffer_pos < buffer_size)
        {
            str[j++] = buffer[buffer_pos++];
            if (str[j - 1] == '\0' || str[j - 1] == 10)
            {
                j--;
                max_len = j;
                break;
            }
        }
    }
    str[j] = 0;
	for(int i=0;i<j+1;i++) master_str[i]=str[i];
	for(int s=0;s<j+1;s++) temp[s]=str[s];
    //printf("%s\n",str);

	return ret;
}
