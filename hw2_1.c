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

int main()
{
    int Mfd = open("/proc/cpuinfo",O_RDONLY);
    char Mstr[100];
    //ReadTextLine(Mfd,Mstr,10);
    while(1){
		int t = ReadTextLine(Mfd,Mstr,100);
        char *ptr = strtok(master_str, ":");
	    printf("%s\n",ptr);
	    ptr = strtok(NULL,"\n");
	    printf("%s\n",ptr);
		//printf("%d\n",t);
		if(t!=0) break;
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

    //printf("%s\n",str);

	return ret;
}
