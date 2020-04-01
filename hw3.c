#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

typedef struct {
    long    type;
    char    buff[BUFFER_SIZE];
} t_data;

int repeat_receiver = 1;
char string_buffer[BUFFER_SIZE];
int num=0;
t_data data;

//pthread_mutex_t mutex =  PTHREAD_MUTEX_INITIALIZER;

void *receiver(void *);
void *sender(void *);

int main(int argc, char *argv[])
{
    int sqid, rqid;
    pthread_t stid, rtid;
    pthread_attr_t sattr, rattr;

    if (argc < 3)
    {
        fprintf(stderr, "Usage: ./hw3 <snd_key> <rcv_key>\n");
        exit(0);
    }
    if (atoi(argv[1]) < 0 || atoi(argv[2]) < 0)
    {
        fprintf(stderr, "%d must be <= 0\n", atoi(argv[1]));
        exit(0);
    }

    

     sqid = msgget((key_t)atoi(argv[1]), IPC_CREAT | 0666);
    if (sqid == -1)
    {
        perror("msgget error : ");
        exit(0);
    }

    rqid = msgget((key_t)atoi(argv[2]), IPC_CREAT | 0666);
    if (rqid == -1)
    {
        perror("msgget error : ");
        exit(0);
    }

    pthread_attr_init(&sattr);
    // pthread_attr_init(&rattr);
    pthread_create(&stid, &sattr, sender, &sqid);
    // pthread_create(&rtid, &rattr, receiver, &rqid);
    pthread_join(stid, NULL);
    // pthread_join(rtid, NULL);

    msgctl(sqid, IPC_RMID, 0);
    // msgctl(rqid, IPC_RMID, 0);

    return 0;
}

void *sender(void *param)
{

    int ret=0;
    
    while (strcmp(string_buffer, "quit") != 0)
    {
    
        printf("[msg] ");
        fgets(string_buffer, BUFFER_SIZE, stdin);
        string_buffer[strlen(string_buffer) - 1] = '\0';
        printf("%d\n",(int)strlen(string_buffer));

        data.type=1;
        strcpy(data.buff,string_buffer);

        printf("%s\n",data.buff);

        if (strcmp(string_buffer, "quit") == 0)
        {
            printf("aaaaaa\n");
            pthread_exit(0);
        }
        //pthread_mutex_loc(&mutex);
        ret = msgsnd(*(int*)param,&data,sizeof(t_data),0);
         if (ret== -1)
            {
                printf("error\n");
                perror("msgsnd() 실패");
                exit(1);
            }


        printf("Suceess\n");
        //pthread_mutex_unlock(&mutex);
        fflush(stderr);
        
    }
    repeat_receiver = 1;
    pthread_exit(0);
}

void *receiver(void *param)
{


    printf("%d",atoi(param));

    while (repeat_receiver == 1)
    {  
         fflush(stderr);
        if (msgrcv(*(int*)param, &data, (sizeof(t_data)-sizeof(long)), 0, IPC_NOWAIT) == -1)
            {
                perror("msgrcv() 실패");
                exit(1);
            }

        printf("%s\n",data.buff);
        
        //usleep(1000);

    }
    pthread_exit(0);
}
