CC = gcc
CFLAGS = -Wall -Wextra -g

SRC = main.c \
	  readFile.c \
	  writeFile.c \
	  partition.c \
	  utils.c \
	  dijkstra.c \
	  kernighanLin.c

OBJ = $(SRC:.c=.o)
TARGET = Cut

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

%.o: %.c 
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean