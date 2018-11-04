CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: HW1

kplc: HW1.o
	${CC} HW1.o -o kplc

main.o: HW1.c
	${CC} ${CFLAGS} HW1.c


clean:
	rm -f *.o *~
