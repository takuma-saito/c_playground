#!/bin/bash -xe
nasm hello.asm -o /dev/stdout |
    ./payload.sh
