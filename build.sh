#!/bin/sh -e
g++ main.cxx -o rawr \
 -std=c++0x -O2 \
 -Werror -Wall -Wextra \
 -lGL -lGLU -lglut 
