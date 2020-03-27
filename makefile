a	:	at.o
	gcc	-o	a	at.o
at.o	:	hw2_1.c
	gcc	-c	-o	at.o	hw2_1.c
clean	:
	rm	*.o	a