CC=gcc
CFLAGS=-Werror -Wall -Wextra -Wno-unused-parameter
release: CFLAGS += -O2
debug: CFLAGS += -g
sanitize_addr: CFLAGS += -fsanitize=address
LIBS=-lGLEW -lGL -lGLU -lglut -lm

all: menger

release: menger

debug: menger

sanitize_addr: menger

menger: main.c vector_math.h
	$(CC) $< -o $@ $(CFLAGS) $(LIBS)

run: menger
	./$<

clean:
	rm -rf menger
