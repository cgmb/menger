#!/bin/sh -e
gcc main.c -o menger \
 -O2 \
 -Werror -Wall -Wextra -Wno-unused-parameter \
 -lGLEW -lGL -lGLU -lglut -lm
