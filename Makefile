CC = gcc

SRC = main.c \
	  readFile.c \
	  writeFile.c \
	  partition.c \
	  utils.c \
	  dijkstra.c \
	  kernighanLin.c \

OBJ = $(SRC:.c=.o)
TARGET = cut

# Default flags
CFLAGS = -Wall -Wextra
LDFLAGS = -lm

.PHONY: all debug release clean test

# Default target is release
all: release

debug: CFLAGS += -g -O0
debug: $(TARGET)

release: CFLAGS += -O2
release: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

TEST_TARGET = test
TEST_SRC = test.c readFile.c partition.c utils.c dijkstra.c kernighanLin.c 
TEST_OBJ = $(TEST_SRC:.c=.o)

test: $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJ) -lm