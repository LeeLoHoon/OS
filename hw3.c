#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define BUFFER_SIZE 512

int repeat_receiver = 1;
char string_buffer[BUFFER_SIZE];
void *receiver(void *);
void *sender(void *);

int main(int argc, char *argv[])
{

    key_t sqid, rqid;
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

    sqid = msgget(atoi(argv[1]), IPC_CREAT | 0666);
    if (sqid == -1)
    {
        perror("msgget error : ");
        exit(0);
    }

    rqid = msgget(atoi(argv[2]), IPC_CREAT | 0666);
    if (rqid == -1)
    {
        perror("msgget error : ");
        exit(0);
    }

    printf("111111111111111");

    pthread_attr_init(&sattr);
    pthread_attr_init(&rattr);
    printf("2222222222222222");
    pthread_create(&stid, &sattr, sender, sqid);
    pthread_create(&rtid, &rattr, receiver, rqid);
    printf("333333333333333333");
    pthread_join(stid, NULL);
    pthread_join(rtid, NULL);

    msgctl(sqid, IPC_RMID, 0);
    msgctl(rqid, IPC_RMID, 0);

    return 0;
}

void *sender(void *param)
{

    while (strcmp(string_buffer, "quit") == 0)
    {
        printf("[msg] ");
        fgets(string_buffer, sizeof(string_buffer), stdin);
        string_buffer[strlen(string_buffer) - 1] = '\0';
        if (strcmp(string_buffer, "quit") == 1)
        {
            if (-1 == msgsnd(atoi(param),string_buffer,0,0))
            {
                perror("msgsnd() 실패");
                exit(1);
            }
            sleep(1);
        }
    }
    repeat_receiver = 1;
    pthread_exit(0);
}

void *receiver(void *param)
{

    while (repeat_receiver == 1)
    {
        if (-1 == msgrcv(atoi(param),string_buffer,BUFFER_SIZE,0,IPC_NOWAIT))
            {
                perror("msgrcv() 실패");
                exit(1);
            }
       usleep(1000);

    }
    pthread_exit(0);
}
