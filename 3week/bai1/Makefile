CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all : client server

client: udpclient.c
	gcc -o client udpclient.c

server: udpserver.c
	gcc -o server udpserver.c

clean:
	rm -f *.o *~