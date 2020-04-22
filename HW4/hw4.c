#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include <unistd.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

#include "Console.h"

// use this structure to send parameter to the child threads
typedef struct {
	int width, height;		// screen size
	int x, y;				// current coordinate of the ball
	int dx, dy;				// velocity of the ball
} ThreadParam;

// initialize the parameter for the thread function
void InitParam(ThreadParam *param, int width, int height);

// thread function to move a ball
void* MoveBall(void *vparam);

int repeat = TRUE;

int main(int argc, char *argv[])
{
	// get <# of balls> from command line parameter
	int no_thread = 0;
	if(argc > 1)
		no_thread = atoi(argv[1]);
	if(no_thread == 0)				// for safety
		no_thread = 5;				// default value of <# of threads>

	srand(time(NULL));

	int width = getWindowWidth();
	int height = getWindowHeight() - 1;
	int t = 0;						// thread index

	EnableCursor(0);				// hide cursor

	clrscr();
	printf("screen size = %d x %d\n", width, height);
	printf("Press ESC to quit!\n");

	ThreadParam param[no_thread];
	for(int i=0; i<no_thread; i++)
		InitParam( &param[i], width, height);


// TO DO: modify the above code to represent multiple balls
//	1. Declare an array of ThreadParam whose length is no_thread.
//		ex) ThreadParam param[no_thread];
//  2. Initialize each of param[t] by calling InitParam()

	pthread_t tid[no_thread];
	pthread_attr_t attr[no_thread];

	for(int j=0;j<no_thread;j++)
		pthread_attr_init(&attr[j]);

	for(int k=0;k<no_thread;k++)
		pthread_create(&tid[k],&attr[k],MoveBall,&param[k]);

	while(getch() != 27) MySleep(1000);
	repeat=FALSE;

	for(int s=0; s<no_thread; s++)
		pthread_join(tid[s],NULL);


// TO DO: extend the above animation code to animate multiple balls using threads 

//	1. Move the above while-loop to the 'void* MoveBall(void *vparam);' below.

//	2. Launch threads using MoveBall() function passing &param[t]

//	3. Wait for ESC key
//		while(getch() != 27)
//			MySleep(1000);

//	4. Terminate the child threads by setting repeat to FALSE (0)

//  5. Wait for the child threads to terminate (call pthread_join())

	clrscr();
	gotoxy(1, 1);
	printf("Bye!\n");

	EnableCursor(1);			// enable cursor

	return 0;
}

void InitParam(ThreadParam *param, int width, int height)
{
	param->width = width;
	param->height = height;
	param->x = rand() % width + 1;
	param->y = rand() % height + 1;
	param->dx = rand() % 7 - 3;
	param->dy = rand() % 5 - 2;
}

void* MoveBall(void *vparam)
{
	ThreadParam *param = (ThreadParam*) vparam;


	while(repeat){
		// if the user presses ESC, terminate
		// DO NOT copy the next 4 lines to MoveBall()


		
		// save current coordinate
		int oldx = param->x;
		int oldy = param->y;
		
		// update horizontal coordinate
		param->x += param->dx;

		// horizontal bouncing
		if(param->x <= 0){
			param->x = 1 + (1 - param->x);
			param->dx = -param->dx;
		} else if(param->x > param->width){
			param->x = param->width - (param->x - param->width) - 1;
			param->dx = -param->dx;
		}

		// update vertical coordinate
		param->y += param->dy;

		// vertical bouncing
		if(param->y <= 0){
			param->y = 1 + (1 - param->y);
			param->dy = -param->dy;
		} else if(param->y > param->height){
			param->y = param->height - (param->y - param->height) - 1;
			param->dy = -param->dy;
		}

		// delete previous ball
		PrintXY(oldx, oldy, " ");

		// draw new ball
		PrintXY(param->x, param->y, "*");



		// delay

		MySleep(50);
	}

	pthread_exit(0);

	// TO DO: implement this function 
	//	move the animation code (the while-loop in the main()) here and modify it to work correctly
	//	delete the if(kbhit()) part

	return NULL;
}

