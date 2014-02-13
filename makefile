CC=gcc
CFLAGS=-O2 \
-Werror -Wall -Wextra -Wno-unused-parameter
LIBS=-lGLEW -lGL -lGLU -lglut -lm

all: menger

menger: main.c vector_math.h
	$(CC) $< -o $@ $(CFLAGS) $(LIBS)

run: menger
	./$<

clean:
	rm -rf menger
