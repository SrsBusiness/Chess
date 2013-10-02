CC=g++
CFLAGS=-Wall
LDFLAGS=-lpanel -lncursesw -lncurses

all: chess

chess: main.cpp chess.h
	$(CC) $(CFLAGS) main.cpp chess.cpp -o chess $(LDFLAGS)

clean:
	rm chess
