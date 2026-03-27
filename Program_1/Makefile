CC = gcc
CFLAGS = -Wall -Wextra -pedantic
TARGET = main
SOURCES = main.c utilities.c
HEADERS = utilities.h constants.h

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
