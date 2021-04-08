#!/bin/bash
cd "$(dirname "$0")"

rm snake.out

clang snake.c -o snake.out
./snake.out