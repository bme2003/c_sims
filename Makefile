CC = gcc
CFLAGS = -Wall -Wextra -pedantic
TARGET = Program_1/main
SOURCES = Program_1/main.c Program_1/utilities.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
