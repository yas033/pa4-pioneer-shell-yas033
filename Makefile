# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=gnu99 -Wno-unused -Werror=vla -g

# Source files
SRC = pish.c pish_history.c

# Object files
OBJ = $(SRC:.c=.o)

# Executable name
TARGET = pish

# Default rule
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean rule
clean:
	rm -f $(OBJ) $(TARGET)
