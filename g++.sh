#!/bin/sh

g++ $1 -o $1.exe -Wextra -Wall -Werror -Wpedantic -std=c++14 -O2 -g
