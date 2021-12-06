#!/usr/bin/env bash

set -e

CC="clang"
CFLAGS="-std=c99 -g -Wall -Werror -pedantic"

echo "Compiling..."
[ -f ./part1.c ] && ${CC} ${CFLAGS} -o part1 part1.c
[ -f ./part2.c ] && ${CC} ${CFLAGS} -o part2 part2.c
echo "Finished"
