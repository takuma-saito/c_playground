#!/bin/bash -xe
nasm -f macho64 hello.asm
# gcc hello.o
ld -e _main -macosx_version_min 10.13 -arch x86_64 hello.o -lSystem -o hello
./hello
