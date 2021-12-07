#!/usr/bin/env bash

set -e

CC="clang"
CFLAGS="-std=c99 -g -Wall -Werror -pedantic"

echo "Compiling..."
count=0
[ -f ./part1.c ] && ${CC} ${CFLAGS} -o part1 part1.c && let count+=1
[ -f ./part2.c ] && ${CC} ${CFLAGS} -o part2 part2.c && let count+=1
echo "Finished (compiled $count files)"

exit 0
