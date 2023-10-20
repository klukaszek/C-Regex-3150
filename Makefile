# Makefile for compiling a2.c

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Source files
SRC = a2.c
HEADER = a2.h

# Output executable
OUT = a2

all: $(OUT)

$(OUT): $(SRC) $(HEADER)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)