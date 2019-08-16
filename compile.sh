#!/bin/bash -xe
nasm -f macho64 hello.asm
ld -e _main -lSystem -o hello hello.o
./hello
