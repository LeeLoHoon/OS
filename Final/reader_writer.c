#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/ipc.h>
// #include <sys/types.h>
// #include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define MAX_MESSAGE 64
char buffer[BUFFER_SIZE][MAX_MESSAGE];
char book[MAX_MESSAGE];
int in = 0, out = 0, count = 0;
int readcount=0;

//pthread_mutex_t mutex;
sem_t wrt,mutex;
int write_num;

int repeat = 1;

void DisplayBuffer();
void *Writer();
void *Reader();

int main(int argc, char *argv[])
{

    int duration;
    if (argc < 2)
        duration = 30;

    else
        duration = atoi(argv[1]);

    srand(time(NULL));

    sem_init(&wrt, 0, 1);
	sem_init(&mutex,0,1);
    //pthread_mutex_init(&mutex, NULL);

    //DisplayBuffer();

    pthread_t write, read;
    pthread_attr_t write_attr, read_attr;

    pthread_attr_init(&write_attr);
    pthread_attr_init(&read_attr);

    pthread_create(&write, &write_attr, Writer, NULL);
    pthread_create(&read, &read_attr, Reader, NULL);

    sleep(duration);

    repeat = 0;

	
	if (sem_getvalue(&wrt, &write_num) == 0)
    {   
        printf("success getwrt\n");
        printf("write_num:%d\n",write_num);
    }   
    if(write_num==0){
        sem_post(&wrt);
        printf("success postwrtl\n");
    }   

   

    pthread_join(write, NULL);
    pthread_join(read, NULL);
    sem_destroy(&mutex);
    sem_destroy(&wrt);

    printf("Bye!\n");
}

void *Writer()
{
    int no_messages = 10;
    char *messages[64] = {
        "Nice to see you!",
        "Aal izz well!",
        "I love you! God loves you!",
        "God loves you and has a wonderful plan for your life.", "God bless you!",
        "You are genius!",
        "Cheer up!",
        "Everything is gonna be okay.",
        "You are so precious!",
        "Hakuna matata!"};
    do
    {
        sleep(rand() % 3 + 1);
        char item[MAX_MESSAGE];
        int num = rand() % 10;
        strcpy(item, messages[num]);
        printf("[Writer] Write a message \"%s \" \n", item);

        sem_wait(&wrt);

        printf("---------------------- WRITER ------------------------\n");
        printf("Writer is entering critical section.\n");
        strcpy(book, item);
        printf("[Writer] \"%s\" ==> buffer\n", item);
        //DisplayBuffer();
        printf("Writer is leaving critical section.\n");
        printf("--------------------------------------------------------\n");

        sem_post(&wrt);
    } while (repeat == 1);

    pthread_exit(0);
}

void *Reader()
{
    do
    {
        char mesg[MAX_MESSAGE];

        sem_wait(&mutex);
		readcount++;
		if(readcount==1)
			sem_wait(&wrt);
		sem_post(&mutex);
		
        printf("--------------------- READER  -------------------------\n");
        printf("Reader is entering critical section.\n");
        strcpy(mesg, book);
        printf("[Reader] buffer ==> \"%s\" ==> buffer\n", mesg);
       // DisplayBuffer();
        printf("Reader is leaving critical section.\n");
        printf("--------------------------------------------------------\n");

		sem_wait(&mutex);
		readcount--;
		if(readcount==0)
			sem_post(&wrt);
        sem_post(&mutex);

        printf("[Reader] readed a message \"%s\" \n", mesg);
        sleep(rand() % 3 + 2);
    } while (repeat == 1);
    pthread_exit(0);
}

void DisplayBuffer()
{
    printf("Buffer contents:\n");
    printf("\tcount = %d, in = %d, out = %d\n", count, in, out);
    int p = out;
    for (int i = 0; i < count; i++)
    {
        printf("\t%d] %s\n", p, buffer[p]);
        p = (p + 1) % BUFFER_SIZE;
    }
}

