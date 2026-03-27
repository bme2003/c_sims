CC = gcc
CFLAGS = -Wall -Wextra -pedantic
TARGET = main

SOURCES = main.c utilities.c builtins.c parser.c
HEADERS = constants.h utilities.h builtins.h parser.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
