#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

static void* (*real_malloc)(size_t);
void* malloc(size_t size) {
  write(STDOUT_FILENO, "init code!\n", 11);
  real_malloc = (void* (*)(size_t)) dlsym(RTLD_NEXT, "malloc");
  if (NULL == real_malloc) {
    fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    return NULL;
  }
  write(STDOUT_FILENO, "This is a wrapper function for malloc.\n", 40);
  return real_malloc(size);
}
