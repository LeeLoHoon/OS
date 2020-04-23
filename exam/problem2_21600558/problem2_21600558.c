// If the comments are hard to read because of the color,
// type ':set background=dark'

/***

	This program reads texts and switches the case of the characters (lower case <-> upper case) until the user types "quit".

	Example)
		Input a text: Welcome!
		convered text = wELCOME!
		Input a text: God bless you!
		convered text = gOD BLESS YOU!
		Input a text: quit
		Bye!

	It launches a child process and communicates with it through two ordinary pipes, one to send the original text and the other to receive the converted text.

	Complete the program using ORDINARY PIPES by following the instructions.
	DO NOT use other IPC such as shared memory or message queue.

	The main creates two pipes and a child process.
	
	Then, the parent process repeats the followings until the user types "quit".
		Read a text line from the user.
		Send the text to the child through pipe.
		Receive and display the converted text.

	The child process repeats the followings until it receives "quit" from the parent. 
		Read a text line from the parent through one pipe.
		Convert all upper case characters to the corresponding lower case characters and vice versa.
		Write the converted text to the parent through the other pipe.

	Hint) To read a text line from the user, use the following code.
			printf("Input a text: ");
			fgets(src, BUFFER_SIZE - 1, stdin);	
			int len = strlen(src) - 1;
			src[len] = 0;				// trim '\n'

	Note! Before the parent terminates, there the child processing MUST terminate.
		You can check whether the child is still running by check currently running processes by 'ps -al'.

***/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdbool.h>

#define TRUE 1
#define FALSE 0

#define READ_END 0
#define WRITE_END 1

#define BUFFER_SIZE 256

#define KEY 8361

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

	//	TO DO: Create two ordinary pipes

	//  TO DO: Create child process

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

		while (1)
		{

			printf("Input a text: ");
			fgets(message, BUFFER_SIZE - 1, stdin);
			int len = strlen(message) - 1;
			message[len] = 0;
			close(in[READ_END]);
			close(out[WRITE_END]);
			write(in[WRITE_END], message, strlen(message) + 1);

			fp = popen("ps -al", "r");
			if (fp == NULL)
			{
				fprintf(stderr, "popen failed\n");
				exit(1);
			}
			while (fgets(line, 4096, fp) != NULL)
			{
				int i = 0;
				char *ptr = strtok(line, " ");
				while (ptr != NULL)
				{
					split[i] = ptr;
					i++;
					ptr = strtok(NULL, " ");
				}
				if (strcmp(split[0], "1") == 0)
					if (strncmp(split[i - 1], "<defunct>", 8) == 0)
					{
						check = true;
						break;
					}
			}
			pclose(fp);

			if (check == true)
			{
				break;
			}

			len = read(out[READ_END], message, BUFFER_SIZE);
			printf("%s\n", message);
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

	// On parent process,
	// Repeats the followings until the user types "quit".
	// Read a text line from the user.
	// Send the text to the child through pipe.
	// Receive and display the converted text.

	// Wait for the child to terminate
	// Deallocate pipes
	// Print a good-bye message

	// On child process call SwitchCase(in, out);
}

void SwitchCase(int in[], int out[])
{
	char buffer[BUFFER_SIZE];
	int len;

	while (1)
	{
		close(in[WRITE_END]);
		close(out[READ_END]);
		len = read(in[READ_END], buffer, BUFFER_SIZE);
		if (strncmp(buffer, "quit", 4) == 0)
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

	// TO DO: Implement the following algorithm
	// Repeats the followings until it receives "quit" from the parent.
	//	Receive a text line from the parent through pipe in.
	//	Convert all upper case characters to the corresponding lower case characters and vice versa.
	// 		You may use isupper(), islower(), toupper(), tolower() functions.
	//		But DO NOT use other IPC such as shared memory or message queue.
	//	Send the converted text to the parent through pipe out.

	// TO DO: deallocate pipes;
}
