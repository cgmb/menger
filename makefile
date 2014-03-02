CC=gcc
override CFLAGS += -Werror -Wall -Wextra -Wno-unused-parameter
release: CFLAGS += -O2
debug: CFLAGS += -g
LIBS=-lGLEW -lGL -lGLU -lglut -lm
EXENAME=menger

all: release

release: menger

debug: menger

menger: main.c vector_math.h vector_debug.h
	$(CC) $< -o $(EXENAME) $(CFLAGS) $(LIBS)

run:
	./$(EXENAME)

clean:
	rm -rf $(EXENAME)
