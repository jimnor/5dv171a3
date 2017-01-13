CC=gcc
FLAGS=-std=gnu99 -Wall -g -lpthread

all:shell

shell: main.o
	$(CC) $(FLAGS) main.o -o main

main.o: main.c
	$(CC) $(FLAGS) -c main.c

clean:
	rm -f *o
