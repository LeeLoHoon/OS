a	:	at.o
	gcc	-o	a	at.o
at.o	:	hw3.c
	gcc	-c	-o	at.o	hw3.c	
clean	:
	rm	*.o	a
