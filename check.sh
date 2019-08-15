#!/bin/bash -xe
filename=${1:-hello.asm}
nasm "${filename}" -o /dev/stdout |
    ./eval_reader
