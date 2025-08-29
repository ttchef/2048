
CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS = 

CFILES = $(wildcard *.c)
OFILES = $(CFILES:.c=.o)
TARGET = main

all: run
	

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(TARGET): $(OFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)


