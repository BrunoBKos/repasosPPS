#!/usr/bin/bash
function programa {
if [ -e "main.c" ]; then
    if [ -e "compr.c" ]; then
        gcc -Wall -Wextra -Werror -ansi -pedantic main.c compr.c -o programa
        exit 0
    else 
        exit 1
    fi
else
    exit 1
fi
}
function limpia {
    if [ -e "programa" ]; then
        rm ./programa
        exit 0
    fi
        exit 1
}

if [ $1 = "limpia" ]; then
    limpia
elif [ $1 = "programa" ]; then
    programa
fi