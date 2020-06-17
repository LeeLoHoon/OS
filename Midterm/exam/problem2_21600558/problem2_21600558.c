#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0

#define READ_END 0
#define WRITE_END 1

#define BUFFER_SIZE 256

void SwitchCase(int in[], int out[]);

int main()
{
	int in[2];	// pipe from parent to child
	int out[2]; // pipe from child to parent
	char message[BUFFER_SIZE];
	FILE *fp;
	char *split[30] = {NULL};
	pid_t pid;
	char line[4096];
	bool check = false;
	int status;

	if (pipe(in) == -1)
	{
		fprintf(stderr, "Pipe failed");
		return 1;
	}

	if (pipe(out) == -1)
	{
		fprintf(stderr, "Pipe failed");
		return 1;
	}

	pid = fork();

	if (pid < 0)
	{
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	else if (pid > 0)
	{

		int wid;

		while (1)
		{

			wid = waitpid(pid, NULL, WNOHANG);
			if (wid != 0)
				break;

			printf("Input a text: ");
			fgets(message, BUFFER_SIZE - 1, stdin);
			int len = strlen(message) - 1;
			message[len] = 0;
			close(in[READ_END]);
			close(out[WRITE_END]);
			write(in[WRITE_END], message, strlen(message) + 1);

			// fp = popen("ps -al", "r");
			// if (fp == NULL)
			// {
			// 	fprintf(stderr, "popen failed\n");
			// 	exit(1);
			// }
			// while (fgets(line, 4096, fp) != NULL) printf("%s\n",line);
			// pclose(fp);

			read(out[READ_END], message, BUFFER_SIZE);
			if (strcmp(message, "quit") == 0)
				break;
			printf("Convered text = %s\n", message);
		}

		close(out[READ_END]);
		close(in[WRITE_END]);
		printf("Bye\n");
	}
	else
	{
		SwitchCase(in, out);
		exit(3);
	}
	return 0;
}

void SwitchCase(int in[], int out[])
{
	char buffer[BUFFER_SIZE];
	int len;

	while (1)
	{
		close(in[WRITE_END]);
		close(out[READ_END]);
		read(in[READ_END], buffer, BUFFER_SIZE);

		if (strcmp(buffer, "quit") == 0)
			break;
		for (int i = 0; i < strlen(buffer); i++)
		{
			if (isupper(buffer[i]))
			{
				buffer[i] = tolower(buffer[i]);
			}
			else
				buffer[i] = toupper(buffer[i]);
		}
		write(out[WRITE_END], buffer, strlen(buffer) + 1);
	}
	close(in[READ_END]);
	close(out[WRITE_END]);
}
