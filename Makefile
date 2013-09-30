CC=g++
CFLAGS=-Wall
LDFLAGS=-lncursesw

all: chess

chess: main.cpp chess.h
	$(CC) $(CFLAGS) main.cpp -o chess $(LDFLAGS)

clean:
	rm chess
