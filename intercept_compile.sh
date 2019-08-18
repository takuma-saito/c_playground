#!/bin/bash -xe
clang -dynamiclib -std=c99 -fPIC intercept.c -o libintercept.dylib
LD_DEBUG=files DYLD_INSERT_LIBRARIES=./libintercept.dylib DYLD_PRINT_APIS=true DYLD_PRINT_LIBRARIES=true ./test_malloc
