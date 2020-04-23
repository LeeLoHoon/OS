#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pthread.h>

#include "Console.h"

#define TRUE 1
#define FALSE 0

int repeat = TRUE;

typedef struct
{
	int width, height;
	int x, y;
	int dx;
	int len;
	char c;
} ThreadParam;

void InitParam(ThreadParam *param, int width, int height, char c, int no_bar);
void *MoveBar(void *vparam);
void DrawBar(int x, int y, int len, char c);

int main(int argc, char *argv[])
{
	int no_bar = 0;

	if (argc > 1)
		no_bar = atoi(argv[1]);
	if (no_bar == 0)
		no_bar = 5;

	srand(time(NULL));

	int width = getWindowWidth();
	int height = getWindowHeight() - 1;
	int t = 0;

	EnableCursor(0);

	clrscr();
	gotoxy(1, 1);
	printf("Press 'q' to quit.\n");

	ThreadParam param[no_bar];
	for (int i = 0; i < no_bar; i++)
		InitParam(&param[i], width, height, 'A' + i, no_bar);

	pthread_t tid[no_bar];
	pthread_attr_t attr[no_bar];

	for (int j = 0; j < no_bar; j++)
		pthread_attr_init(&attr[j]);

	for (int k = 0; k < no_bar; k++)
		pthread_create(&tid[k], &attr[k], MoveBar, &param[k]);

	while (getch() != 'q')
		MySleep(1000);
	repeat = FALSE;

	for (int s = 0; s < no_bar; s++)
		pthread_join(tid[s], NULL);

	EnableCursor(FALSE);

	gotoxy(1, 1);
	printf("Bye!\n");

	EnableCursor(TRUE);

	return 0;
}

void InitParam(ThreadParam *param, int width, int height, char c, int no_bar)
{

	param->width = width;
	param->height = height;
	param->c = c;
	param->len = rand() % 6 + 5; // a random number in [5, 10]
	int step = rand() % 4 + 2;	 // a random number in [2, 5]
	param->x = rand() % (param->width - step - 1) + 1;

	// vertical interval between bars = height / (no_bar + 1)

	float interval = param->height / (float)(no_bar + 1);

	param->y = (int)interval * (param->c - 64);

	param->dx = step;
	if (rand() % 2 == 0)
		param->dx = -param->dx;
}

void *MoveBar(void *vparam)
{

	ThreadParam *param = (ThreadParam *)vparam;

	DrawBar(param->x, param->y, param->len, param->c);

	while (repeat)
	{
		int oldx = param->x;

		param->x += param->dx;
		if (param->x <= 0)
		{
			param->dx = -param->dx;
			param->x = 1 - param->x;
		}

		if (param->x > param->width - param->len + 1)
		{
			param->dx = -param->dx;
			param->x = param->width - param->len + 1 - (param->x - param->width + param->len - 1);
		}

		DrawBar(oldx, param->y, param->len, ' ');
		DrawBar(param->x, param->y, param->len, param->c);

		MySleep(50);
	}
}

void DrawBar(int x, int y, int len, char c)
{
	pthread_mutex_lock(&display_mutex);
	gotoxy(x, y);
	for (int i = 0; i < len; i++)
		putchar(c);
	fflush(stdout);

	pthread_mutex_unlock(&display_mutex);
}
