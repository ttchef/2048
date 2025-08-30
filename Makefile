CC = gcc
CFLAGS = -Wall -Werror -g
LDFLAGS = -lraylib -lGL -lm -lpthread -lX11 -lXrandr -lXi -lXcursor -lXinerama -lwayland-client -lXext -lEGL # These flags are for Raylib
TARGET = 2048

SRCS = src/main.c
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

all: $(TARGET)

build:
	@mkdir -p build

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build $(TARGET)

cloc:
	cloc . --exclude-dir=vendor,build,third_party