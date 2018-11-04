CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all : client server

client: tcpcl.c
	gcc -o client tcpcl.c

server: tcpsv.c
	gcc -o server tcpsv.c

clean:
	rm -f *.o *~