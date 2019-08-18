#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

static void* (*real_malloc)(size_t);
static char* libsym = "/usr/lib/libSystem.dylib";

void* malloc(size_t size) {
  void* handler = dlopen(libsym, RTLD_NOW);
  write(STDOUT_FILENO, "init code!\n", 11);
  real_malloc = (void* (*)(size_t)) dlsym(handler, "malloc");
  if (NULL == real_malloc) {
    fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    return NULL;
  }
  write(STDOUT_FILENO, "This is a wrapper function for malloc.\n", 40);
  return real_malloc(size);
}

int main() {
  void* m = malloc(128);
  exit(0);
}
