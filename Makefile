#
# Makefile for tictactoe
#
CC = gcc 
CFLAGS = -Wall -g 

.PHONY: default
default: client server

client: client.h tic-tac-toe.h tic-tac-toe.c client.c
	$(CC) $(CFLAGS) tic-tac-toe.c client.c -o client

server: server.h tic-tac-toe.h tic-tac-toe.c server.c
	$(CC) $(CFLAGS) tic-tac-toe.c server.c -o server

run: 
	./server &
	./client

clean:
	rm -f client server

