#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

static void* (*real_malloc)(size_t);


__attribute__((constructor))
static void init() {
  real_malloc = (void* (*)(size_t)) dlsym(RTLD_NEXT, "malloc");
}

main() {
  printf("%p\n", real_malloc);
}