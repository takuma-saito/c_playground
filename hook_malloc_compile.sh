#!/bin/bash -xe
clang -dynamiclib -undefined suppress -flat_namespace -std=c99 -fPIC hook_malloc.c -o libhookmalloc.dylib
LD_DEBUG=files DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=./libhookmalloc.dylib DYLD_PRINT_APIS=true DYLD_PRINT_LIBRARIES=true ./test_malloc
