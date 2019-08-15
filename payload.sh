#!/bin/bash -xe
cat /dev/stdin |
    xxd -p |
    tr -d '\n' |
    fold -w 2 |
    xargs -I@ echo "\x@" |
    tr -d '\n'
