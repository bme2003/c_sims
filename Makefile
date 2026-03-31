CC = gcc
CFLAGS = -Wall -Wextra -pedantic
TARGET = Program_1/main
SOURCES = Program_1/main.c Program_1/utilities.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

.PHONY: bootloader clean

bootloader:
	$(MAKE) -C bootloader

clean:
	rm -f $(TARGET)
	$(MAKE) -C bootloader clean
