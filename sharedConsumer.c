#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>


#define BUFFER_SIZE 6



typedef struct{
    int in;
    int out;
    char item[6][20];
}entire;

int main(){

    const int SIZE = 4096;
    const char *name = "OSS";
    char chat[20];
    int shm_fd;

    entire *ptr;
    
    shm_fd = shm_open(name, O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(entire));

    ptr = mmap(0,sizeof(entire), PROT_WRITE, MAP_SHARED,  shm_fd,0);
    while(1){
        while(ptr->in==ptr->out);
        strcpy(chat,ptr->item[ptr->out]);
        ptr->out = ((ptr->out)+1)%BUFFER_SIZE;
        printf("%d\n",ptr->in);
        printf("%d\n",ptr->out);
        printf("%s\n",chat);

    }

    shm_unlink(name);

    return 0;
}