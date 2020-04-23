#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_CMD 2048
#define MAX_ARG 256

void ParseCommand(char *command, int *argc, char *argv[]);
int main()
{
    pid_t pid;
    char command[MAX_CMD];
    command[0] = command[MAX_CMD - 1] = 0; // for safety
    int argc = 0;
    char *argv[MAX_ARG] = {NULL};
    while (1)
    {
        printf("$ ");
        fgets(command, MAX_CMD - 1, stdin);
        command[strlen(command) - 1] = 0; // trim \r
        if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0)
            break;
        ParseCommand(command, &argc, argv);
        pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "fork failed\n");
            exit(-1);
        }
        else if (pid == 0)
        {
            execvp(argv[0], argv);
        }
        else
        {
            wait(NULL);
            //printf("Child Completed\n");
            //exit(0);
        }
    }
    printf("Bye!\n");
    return 0;
}

void ParseCommand(char *command, int *argc, char *argv[])
{

    int j = 0;
    int n = 0;
    int temp = 0;
    int len = strlen(command);
    command[len] = 0;
    for (int i = 0; i < len + 1; i++)
    {
        if (command[i] == '"' || command[i] == '\'')
        {
            command[i] = 0;
            argv[n] = command + i + 1;
            n++;
            for (j = i; j < len; j++)
            {
                if (command[j] == '"' || command[j] == '\'')
                {
                    command[j] = 0;
                    i = j;
                    break;
                }
            }
        }
        else if (command[i] == ' ')
        {
            command[i] = 0;
        }
        else if (command[i] == 0)
        {
            argv[n] = 0;
            break;
        }
        else
        {
            argv[n] = command + i;
            n++;
            for (j = i + 1; j < len + 1; j++)
            {
                if (command[j] == ' ' || command[j] == '"' || command[j] == '\'' || command[j] == 0)
                {
                    i = j - 1;
                    break;
                }
            }
        }
    }
    *argc = n;
}