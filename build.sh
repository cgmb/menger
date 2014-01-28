#!/bin/sh -e
gcc main.cxx -o rawr \
 -O2 \
 -Werror -Wall -Wextra -Wno-unused-parameter \
 -lGL -lGLU -lglut 
