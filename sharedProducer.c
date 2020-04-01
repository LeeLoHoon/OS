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
    const char *message_0 = "Hello";
    const char *message_1 = "World!";
    char chat[20];

    int shm_fd;

    entire *ptr;
    
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(entire));
    //printf("%ld\n",sizeof(entire));

    ptr = mmap(0,sizeof(entire), PROT_WRITE, MAP_SHARED,  shm_fd,0);
    ptr->in = 0;
    ptr->out = 0;

    //printf("%d\n",ptr->in);
    while(1){
        
        while((((ptr->in)+1)%BUFFER_SIZE)==ptr->out);
        fgets(chat,sizeof(chat),stdin);
        chat[strlen(chat)-1]='\0';
        strcpy(ptr->item[ptr->in],chat);
        ptr->in = ((ptr->in)+1)%BUFFER_SIZE;
    }




    sprintf(ptr->item[ptr->in], "%s", message_0);
    printf("%s\n",ptr->item[ptr->in]);
    ptr->in++;
    printf("%d\n",ptr->in);

    return 0;
}