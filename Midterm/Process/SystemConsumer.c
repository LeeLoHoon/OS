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

int main(){

    const int SIZE = 4096;
    const char *name = "OS";

    int shm_fd;

    void *ptr;
    
    shm_fd = shmget(IPC_PRIVATE,32,S_IRUSR|S_IWUSR);

    ptr = shmat(shm_fd,NULL,0);

    printf("%s",(char *)ptr);
    shmdt(ptr);
    
    shmctl(shm_fd,IPC_RMID,NULL);

    return 0;
}