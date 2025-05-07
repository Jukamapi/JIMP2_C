TARGET = main.exe

CC = gcc

CFLAGS = -Wall -Wextra -std=c99
DEBUG_FLAGS = -g
RELEASE_FLAGS = -O2

SRC = main.c graph.c io.c flags.c partition.c utils.c dijkstra.c kernighan_lin.c

HEADERS = graph.h io.h flags.h partition.h utils.h dijkstra.h kernighan_lin.h

OBJ = $(SRC:.c=.o)

all: release

debug:
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(SRC) -o $(TARGET)

release:
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(SRC) -o $(TARGET)

run: release
	./$(TARGET)

clean:
	del /Q *.o *.exe 2>nul || true
