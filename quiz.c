#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define  KEY_NUM     9527
#define  MEM_SIZE    1024

// struct for message buffer
typedef struct {
	char message[512];			// message buffer
	int filled;					// 1 if the message is filled. 0 otherwise.
} Buffer;

Buffer global_buffer;

void parent(int shm_id);		// function for the parent
void child(int shm_id);			// function for the child

int main(int argc, char argv[0])
{
	int shm_id = 0;

	if ( -1 == ( shm_id = shmget( (key_t)KEY_NUM, MEM_SIZE, IPC_CREAT|0666))){
      printf( "공유 메모리 생성 실패\n");
      return -1;
   }	

	// hint: allocate shared memory block for the shared buffer

	pid_t child_pid = fork();

	if(child_pid < 0){
		printf("Failed to create child process\n");
		exit(-1);
	} else if(child_pid > 0){
		parent(shm_id);
	} else {
		// child
		child(shm_id);
	}

	shmctl(shm_id,IPC_RMID,NULL);

	return 0;
}

void parent(int shm_id)
{

	
	
	Buffer *buffer = &global_buffer;		// hint: modify this line
	buffer->filled = 0;

	if ( ( void *)-1 == ( buffer= shmat( shm_id, ( void *)0, 0))){
      printf( "공유 메모리 첨부 실패\n");
      return -1;
   }

	sleep(2);			// wait for the child to start
	printf("[parent] Input a message and I'll send to the child.\n");

	while(1){			// DO NOT print any message in this loop.
		char message[512];
		message[511] = 0;	// for safety

		fgets(message, 512, stdin);
		message[strlen(message) - 1] = 0;	// trim '\n'

		while(buffer->filled)
			usleep(50000);

		strcpy(buffer->message, message);
		buffer->filled = 1;

		if(strcmp(message, "quit") == 0)
			break;
	}

	shmdt(buffer);
	// hint: put some code here
	
	printf("[parent] Terminating.\n");
	fflush(stdout);
}

void child(int shm_id)
{
	sleep(1);			// wait for the parent to initialize the shared buffer

	printf("[child] Started\n");
	fflush(stdout);


	Buffer *buffer = &global_buffer;		// hint: modify this line

	if ( ( void *)-1 == ( buffer= shmat( shm_id, ( void *)0, 0))){
      printf( "공유 메모리 첨부 실패\n");
      return -1;
   }
	while(1){
		while(!buffer->filled)
			usleep(50000);

		if(buffer->message[0]){
			printf("[child] %s\n", buffer->message);
			fflush(stdout);
			buffer->filled = 0;
		}

		if(strcmp(buffer->message, "quit") == 0)
			break;
	}

	shmdt(buffer);

	// hint: put some code here

	printf("[child] Terminating.\n");
	fflush(stdout);
}

