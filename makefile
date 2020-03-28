a	:	at.o
	gcc	-o	a	at.o
# b	:	bt.o
# 	gcc	-o	b	bt.o
# c	:	ct.o
#	gcc	-o	c	ct.o
at.o	:	hw2_1.c
	gcc	-c	-o	at.o	hw2_1.c
# bt.o	:	hw2_2.c
# 	gcc	-c	-o	bt.o	hw2_2.c
#ct.o	:	hw2_3.c
#	gcc	-c	-o	ct.o	hw2_3.c	
clean	:
	rm	*.o	a