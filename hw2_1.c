#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int ReadTextLine(int fd, char str[], int max_len);

#define BUFFER_SIZE 512
char buffer[BUFFER_SIZE];
int buffer_size = 0;
int buffer_pos = 0;
char master_str[100];
char temp[100];
int core;
char model[100];
int memory;
float  loadavg1, loadavg5, loadavg15;

int main()
{
    char Mstr[100];
    int Cfd = open("/proc/cpuinfo",O_RDONLY);
    while(1){
		int t = ReadTextLine(Cfd,Mstr,100);
        char *ptr = strtok(temp, ":");

        if(!strncmp("cpu cores",ptr,9)){
			sscanf(master_str,"%*s %*s %*s %d",&core);
			break;
		}
		if(!strncmp("model name",ptr,10)){
			ptr = strtok(NULL,"\n");
			strcpy(model,ptr+1);
		}
    }
	close(Cfd);
    
    int Mfd = open("/proc/meminfo",O_RDONLY);
	ReadTextLine(Mfd,Mstr,100);
    sscanf(master_str,"%*s %d",&memory);
    close(Mfd);

    int Lfd = open("/proc/loadavg",O_RDONLY);
	ReadTextLine(Lfd,Mstr,100);
    sscanf(master_str,"%f %f %f",&loadavg1,&loadavg5,&loadavg15);
    close(Lfd);

    printf("# of processor cores = %d\n",core);
    printf("CPU model = %s\n",model);
    printf("MemTotal = %d\n",memory);
    printf("loadavg1 = %f, loadavg5 = %f, loadavd15 = %f\n",loadavg1,loadavg5,loadavg15);

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

	return ret;
}
