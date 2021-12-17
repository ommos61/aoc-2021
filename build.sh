#!/usr/bin/env bash

set -e

CC="clang"
CFLAGS="-std=c99 -g -Wall -Wextra -Werror -pedantic"

function compile_file {
    src=$1
    target=$2

    [[ -f ${src} ]] && [[ ${src} -nt ${target} ]] && \
        echo "${src} -> ${target}" && ${CC} ${CFLAGS} -o ${target} ${src}
    return $?
}

function compile_day {
    day="$1"

    count=0
    compile_file ${day}/part1.c ${day}/part1 && let count+=1
    compile_file ${day}/part2.c ${day}/part2 && let count+=1
    [[ $count != 0 ]] && echo "Day ${day}: compiled $count files"
    return 0
}

echo "Compiling all new files..."
for d in [01][0-9]
do
    compile_day ${d}
done
echo "Done"

exit 0
