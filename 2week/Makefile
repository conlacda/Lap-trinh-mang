CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: resolver

kplc: resolver.o
	${CC} resolver.o -o kplc

main.o: resolver.c
	${CC} ${CFLAGS} resolver.c


clean:
	rm -f *.o *~
