CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all : client server

client: client.c
	gcc -o client client.c

server: server.c
	gcc -o server server.c

clean:
	rm -f *.o *~