#!/bin/bash
cd "$(dirname "$0")"

rm snake.out

clang -g snake.c -o snake.out
./snake.out