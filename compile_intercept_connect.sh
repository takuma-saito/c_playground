#!/bin/bash -xe
clang -dynamiclib -std=c99 -fPIC intercept_connect.c -o libinterceptConnect.dylib
LD_DEBUG=files DYLD_PRINT_APIS=true DYLD_PRINT_LIBRARIES=true
DYLD_INSERT_LIBRARIES=./libinterceptConnect.dylib ./curl ipcheck.com
