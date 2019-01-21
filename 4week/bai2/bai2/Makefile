CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all : client server

client: tcp_cl.c
	gcc -o client tcp_cl.c

server: tcp_sv.c
	gcc -o server tcp_sv.c

clean:
	rm -f *.o *~