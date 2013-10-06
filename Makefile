CC=g++
CFLAGS=-Wall
LDFLAGS=-lpanel -lncurses

all: chess

chess: main.cpp chess.cpp multiplayer.cpp
	$(CC) $(CFLAGS) main.cpp chess.cpp multiplayer.cpp -o chess $(LDFLAGS)

clean:
	rm chess
