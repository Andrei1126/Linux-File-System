CC = gcc
FLAGS = -g -O2 -Wall -Wextra -std=c99
EXE = tema3

.PHONY: build clean

build: $(EXE)

$(EXE): main.o filemgr.o
	$(CC) $(FLAGS) main.o filemgr.o -o $(EXE)

main.o: main.c filemgr.h
	$(CC) $(FLAGS) main.c -c -o main.o

filemgr.o: filemgr.c filemgr.h
	$(CC) $(FLAGS) filemgr.c -c -o filemgr.o

clean:
	rm $(EXE) main.o filemgr.o
